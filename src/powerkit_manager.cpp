/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit_manager.h"
#include "powerkit_common.h"
#include "powerkit_settings.h"

#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusPendingReply>
#include <QXmlStreamReader>
#include <QProcess>
#include <QMapIterator>
#include <QDebug>
#include <QDBusReply>

#define LOGIND_SERVICE "org.freedesktop.login1"
#define LOGIND_PATH "/org/freedesktop/login1"
#define LOGIND_MANAGER "org.freedesktop.login1.Manager"
#define LOGIND_DOCKED "Docked"

#define UPOWER_PATH "/org/freedesktop/UPower"
#define UPOWER_MANAGER "org.freedesktop.UPower"
#define UPOWER_DEVICES "/org/freedesktop/UPower/devices/"
#define UPOWER_DOCKED "IsDocked"
#define UPOWER_LID_IS_PRESENT "LidIsPresent"
#define UPOWER_LID_IS_CLOSED "LidIsClosed"
#define UPOWER_ON_BATTERY "OnBattery"
#define UPOWER_NOTIFY_RESUME "NotifyResume"
#define UPOWER_NOTIFY_SLEEP "NotifySleep"

#define DBUS_OK_REPLY "yes"
#define DBUS_FAILED_CONN "Failed D-Bus connection."
#define DBUS_OBJECT_MANAGER "org.freedesktop.DBus.ObjectManager"

#define DBUS_INTROSPECTABLE "org.freedesktop.DBus.Introspectable"
#define DBUS_JOBS "%1/jobs"
#define DBUS_DEVICE_ADDED "DeviceAdded"
#define DBUS_DEVICE_REMOVED "DeviceRemoved"
#define DBUS_DEVICE_CHANGED "DeviceChanged"

#define PK_PREPARE_FOR_SUSPEND "PrepareForSuspend"
#define PK_PREPARE_FOR_SLEEP "PrepareForSleep"
#define PK_CAN_RESTART "CanReboot"
#define PK_RESTART "Reboot"
#define PK_CAN_POWEROFF "CanPowerOff"
#define PK_POWEROFF "PowerOff"
#define PK_CAN_SUSPEND "CanSuspend"
#define PK_SUSPEND_ALLOWED "SuspendAllowed"
#define PK_SUSPEND "Suspend"
#define PK_CAN_HIBERNATE "CanHibernate"
#define PK_HIBERNATE_ALLOWED "HibernateAllowed"
#define PK_HIBERNATE "Hibernate"
#define PK_CAN_HYBRIDSLEEP "CanHybridSleep"
#define PK_HYBRIDSLEEP "HybridSleep"
#define PK_NO_BACKEND "No backend available."
#define PK_NO_ACTION "Action no available."

#define TIMEOUT_CHECK 60000

using namespace PowerKit;

Manager::Manager(QObject *parent) : QObject(parent)
  , upower(nullptr)
  , logind(nullptr)
  , pmd(nullptr)
  , wasDocked(false)
  , wasLidClosed(false)
  , wasOnBattery(false)
  , wakeAlarm(false)
  , suspendWakeupBattery(0)
  , suspendWakeupAC(0)
{
    setup();
    timer.setInterval(TIMEOUT_CHECK);
    connect(&timer, SIGNAL(timeout()),
            this, SLOT(check()));
    timer.start();
}

Manager::~Manager()
{
    clearDevices();
    ReleaseSuspendLock();
}

QMap<QString, Device *> Manager::getDevices()
{
    return devices;
}

bool Manager::availableService(const QString &service,
                          const QString &path,
                          const QString &interface)
{
    QDBusInterface iface(service,
                         path,
                         interface,
                         QDBusConnection::systemBus());
    if (iface.isValid()) { return true; }
    return false;
}

bool Manager::availableAction(const Manager::PKMethod &method,
                               const Manager::PKBackend &backend)
{
    QString service, path, interface, cmd;
    switch (backend) {
    case PKLogind:
        service = LOGIND_SERVICE;
        path = LOGIND_PATH;
        interface = LOGIND_MANAGER;
        break;
    case PKUPower:
        service = UPOWER_SERVICE;
        path = UPOWER_PATH;
        interface = UPOWER_MANAGER;
        break;
    default:
        return false;
    }
    switch (method) {
    case PKCanRestart:
        cmd = PK_CAN_RESTART;
        break;
    case PKCanPowerOff:
        cmd = PK_CAN_POWEROFF;
        break;
    case PKCanSuspend:
        cmd = PK_CAN_SUSPEND;
        break;
    case PKCanHibernate:
        cmd = PK_CAN_HIBERNATE;
        break;
    case PKCanHybridSleep:
        cmd = PK_CAN_HYBRIDSLEEP;
        break;
    case PKSuspendAllowed:
        cmd = PK_SUSPEND_ALLOWED;
        break;
    case PKHibernateAllowed:
        cmd = PK_HIBERNATE_ALLOWED;
        break;
    default:
        return false;
    }
    QDBusInterface iface(service, path, interface,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    QDBusMessage reply = iface.call(cmd);
    const auto args = reply.arguments();
    if (args.first().toString() == DBUS_OK_REPLY) { return true; }
    bool result = args.first().toBool();
    if (!reply.errorMessage().isEmpty()) { result = false; }
    return result;
}

QString Manager::executeAction(const Manager::PKAction &action,
                                const Manager::PKBackend &backend)
{
    QString service, path, interface, cmd;
    switch (backend) {
    case PKLogind:
        service = LOGIND_SERVICE;
        path = LOGIND_PATH;
        interface = LOGIND_MANAGER;
        break;
    case PKUPower:
        service = UPOWER_SERVICE;
        path = UPOWER_PATH;
        interface = UPOWER_MANAGER;
        break;
    default:
        return QObject::tr(PK_NO_BACKEND);
    }
    switch (action) {
    case PKRestartAction:
        cmd = PK_RESTART;
        break;
    case PKPowerOffAction:
        cmd = PK_POWEROFF;
        break;
    case PKSuspendAction:
        cmd = PK_SUSPEND;
        break;
    case PKHibernateAction:
        cmd = PK_HIBERNATE;
        break;
    case PKHybridSleepAction:
        cmd = PK_HYBRIDSLEEP;
        break;
    default:
        return QObject::tr(PK_NO_ACTION);
    }
    QDBusInterface iface(service, path, interface,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return QObject::tr(DBUS_FAILED_CONN); }

    QDBusMessage reply;
    if (backend == PKUPower) { reply = iface.call(cmd); }
    else { reply = iface.call(cmd, true); }

    return reply.errorMessage();
}

QStringList Manager::find()
{
    QStringList result;
    QDBusMessage call = QDBusMessage::createMethodCall(UPOWER_SERVICE,
                                                       QString("%1/devices").arg(UPOWER_PATH),
                                                       DBUS_INTROSPECTABLE,
                                                       "Introspect");
    QDBusPendingReply<QString> reply = QDBusConnection::systemBus().call(call);
    if (reply.isError()) {
        qWarning() << "powerkit find devices failed, check the upower service!!!";
        return result;
    }
    QList<QDBusObjectPath> objects;
    QXmlStreamReader xml(reply.value());
    if (xml.hasError()) { return result; }
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name().toString() == "node" ) {
            QString name = xml.attributes().value("name").toString();
            if (!name.isEmpty()) { objects << QDBusObjectPath(UPOWER_DEVICES + name); }
        }
    }
    foreach (QDBusObjectPath device, objects) {
        result << device.path();
    }
    return result;
}

void Manager::setup()
{
    QDBusConnection system = QDBusConnection::systemBus();
    if (system.isConnected()) {
        system.connect(UPOWER_SERVICE,
                       UPOWER_PATH,
                       UPOWER_SERVICE,
                       DBUS_DEVICE_ADDED,
                       this,
                       SLOT(deviceAdded(QDBusObjectPath)));
        system.connect(UPOWER_SERVICE,
                       UPOWER_PATH,
                       UPOWER_SERVICE,
                       DBUS_DEVICE_ADDED,
                       this,
                       SLOT(deviceAdded(QString)));
        system.connect(UPOWER_SERVICE,
                       UPOWER_PATH,
                       UPOWER_SERVICE,
                       DBUS_DEVICE_REMOVED,
                       this,
                       SLOT(deviceRemoved(QDBusObjectPath)));
        system.connect(UPOWER_SERVICE,
                       UPOWER_PATH,
                       UPOWER_SERVICE,
                       DBUS_DEVICE_REMOVED,
                       this,
                       SLOT(deviceRemoved(QString)));
        // not used anymore?
        /*system.connect(UPOWER_SERVICE,
                       UPOWER_PATH,
                       UPOWER_SERVICE,
                       DBUS_CHANGED,
                       this,
                       SLOT(deviceChanged()));
        system.connect(UPOWER_SERVICE,
                       UPOWER_PATH,
                       UPOWER_SERVICE,
                       DBUS_DEVICE_CHANGED,
                       this,
                       SLOT(deviceChanged()));*/
        system.connect(UPOWER_SERVICE,
                       UPOWER_PATH,
                       DBUS_PROPERTIES,
                       DBUS_PROPERTIES_CHANGED,
                       this,
                       SLOT(propertiesChanged()));
        system.connect(UPOWER_SERVICE,
                       UPOWER_PATH,
                       UPOWER_SERVICE,
                       UPOWER_NOTIFY_RESUME,
                       this,
                       SLOT(handleResume()));
        system.connect(UPOWER_SERVICE,
                       UPOWER_PATH,
                       UPOWER_SERVICE,
                       UPOWER_NOTIFY_SLEEP,
                       this,
                       SLOT(handleSuspend()));
        if (upower == NULL) {
            upower = new QDBusInterface(UPOWER_SERVICE,
                                        UPOWER_PATH,
                                        UPOWER_MANAGER,
                                        system,
                                        this);
            qDebug() << "upower" << upower->isValid();
        }
        if (logind == NULL) {
            logind = new QDBusInterface(LOGIND_SERVICE,
                                        LOGIND_PATH,
                                        LOGIND_MANAGER,
                                        system,
                                        this);
            qDebug() << "logind" << logind->isValid();
        }
        if (pmd == NULL) {
            pmd = new QDBusInterface(PMD_SERVICE,
                                     PMD_PATH,
                                     PMD_MANAGER,
                                     system,
                                     this);
            qDebug() << "powerkitd" << pmd->isValid();
        }
        if (logind->isValid()) {
            connect(logind,
                    SIGNAL(PrepareForSleep(bool)),
                    this,
                    SLOT(handlePrepareForSuspend(bool)));
        }
        if (!suspendLock) { registerSuspendLock(); }
        if (!lidLock) { registerLidLock(); }
        scan();
    } else { qWarning() << "Failed to connect to system bus"; }
}

void Manager::check()
{
    if (!QDBusConnection::systemBus().isConnected()) {
        setup();
        return;
    }
    if (!suspendLock) { registerSuspendLock(); }
    if (!lidLock) { registerLidLock(); }
    if (!upower->isValid()) { scan(); }
}

void Manager::scan()
{
    QStringList foundDevices = find();
    for (int i=0; i < foundDevices.size(); i++) {
        QString foundDevicePath = foundDevices.at(i);
        if (devices.contains(foundDevicePath)) { continue; }
        Device *newDevice = new Device(foundDevicePath, this);
        connect(newDevice,
                SIGNAL(deviceChanged(QString)),
                this,
                SLOT(handleDeviceChanged(QString)));
        devices[foundDevicePath] = newDevice;
    }
    UpdateDevices();
    emit UpdatedDevices();
}

void Manager::deviceAdded(const QDBusObjectPath &obj)
{
    deviceAdded(obj.path());
}

void Manager::deviceAdded(const QString &path)
{
    qDebug() << "device added" << path;
    if (!upower->isValid()) { return; }
    if (path.startsWith(QString(DBUS_JOBS).arg(UPOWER_PATH))) { return; }
    emit DeviceWasAdded(path);
    scan();
}

void Manager::deviceRemoved(const QDBusObjectPath &obj)
{
    deviceRemoved(obj.path());
}

void Manager::deviceRemoved(const QString &path)
{
    qDebug() << "device removed" << path;
    if (!upower->isValid()) { return; }
    bool deviceExists = devices.contains(path);
    if (path.startsWith(QString(DBUS_JOBS).arg(UPOWER_PATH))) { return; }
    if (deviceExists) {
        if (find().contains(path)) { return; }
        delete devices.take(path);
        emit DeviceWasRemoved(path);
    }
    scan();
}

void Manager::deviceChanged()
{
    qDebug() << "a device changed, tell the world!";
    emit UpdatedDevices();
}

void Manager::propertiesChanged()
{
    bool isLidClosed = LidIsClosed();
    bool isOnBattery = OnBattery();

    qDebug() << "properties changed:"
             << "lid closed?" << wasLidClosed << isLidClosed
             << "on battery?" << wasOnBattery << isOnBattery;

    if (wasLidClosed != isLidClosed) {
        if (!wasLidClosed && isLidClosed) {
            qDebug() << "lid changed status to closed";
            emit LidClosed();
        } else if (wasLidClosed && !isLidClosed) {
            qDebug() << "lid changed status to open";
            emit LidOpened();
        }
    }
    wasLidClosed = isLidClosed;

    if (wasOnBattery != isOnBattery) {
        if (!wasOnBattery && isOnBattery) {
            qDebug() << "switched to battery power";
            emit SwitchedToBattery();
        } else if (wasOnBattery && !isOnBattery) {
            qDebug() << "switched to ac power";
            emit SwitchedToAC();
        }
    }
    wasOnBattery = OnBattery();

    deviceChanged();
}

void Manager::handleDeviceChanged(const QString &device)
{
    Q_UNUSED(device)
    qDebug() << "device changed" << device;
    deviceChanged();
}

void Manager::handleResume()
{
    if (HasLogind()) { return; }
    qDebug() << "handle resume from upower";
    handlePrepareForSuspend(false);
}

void Manager::handleSuspend()
{
    if (HasLogind()) { return; }
    qDebug() << "handle suspend from upower";
    LockScreen();
    emit PrepareForSuspend();
}

void Manager::handlePrepareForSuspend(bool prepare)
{
    qDebug() << "handle prepare for suspend/resume from logind" << prepare;
    if (prepare) {
        qDebug() << "ZZZ...";
        LockScreen();
        emit PrepareForSuspend();
        QTimer::singleShot(500, this, SLOT(ReleaseSuspendLock()));
    }
    else { // resume
        qDebug() << "WAKE UP!";
        UpdateDevices();
        if (HasWakeAlarm() &&
             wakeAlarmDate.isValid() &&
             CanHibernate())
        {
            qDebug() << "we may have a wake alarm" << wakeAlarmDate;
            QDateTime currentDate = QDateTime::currentDateTime();
            if (currentDate>=wakeAlarmDate && wakeAlarmDate.secsTo(currentDate)<300) {
                qDebug() << "wake alarm is active, that means we should hibernate";
                ClearWakeAlarm();
                Hibernate();
                return;
            }
        }
        ClearWakeAlarm();
        emit PrepareForResume();
        QTimer::singleShot(500, this, SLOT(registerSuspendLock()));
    }
}

void Manager::clearDevices()
{
    QMapIterator<QString, Device*> device(devices);
    while (device.hasNext()) {
        device.next();
        delete device.value();
    }
    devices.clear();
}

void Manager::handleNewInhibitScreenSaver(const QString &application, const QString &reason, quint32 cookie)
{
    qDebug() << "PK HANDLE NEW SCREEN SAVER INHIBITOR" << application << reason << cookie;
    Q_UNUSED(reason)
    ssInhibitors[cookie] = application;
    emit UpdatedInhibitors();
}

void Manager::handleNewInhibitPowerManagement(const QString &application, const QString &reason, quint32 cookie)
{
    qDebug() << "PK HANDLE NEW POWER INHIBITOR" << application << reason << cookie;
    Q_UNUSED(reason)
    pmInhibitors[cookie] = application;
    emit UpdatedInhibitors();
}

void Manager::handleDelInhibitScreenSaver(quint32 cookie)
{
    qDebug() << "PK HANDLE REMOVE SCREEN SAVER COOKIE" << cookie;
    if (ssInhibitors.contains(cookie)) {
        ssInhibitors.remove(cookie);
        emit UpdatedInhibitors();
    }
}

void Manager::handleDelInhibitPowerManagement(quint32 cookie)
{
    qDebug() << "PK HANDLE REMOVE POWER COOKIE" << cookie;
    if (pmInhibitors.contains(cookie)) {
        pmInhibitors.remove(cookie);
        emit UpdatedInhibitors();
    }
}

bool Manager::registerSuspendLock()
{
    if (suspendLock) { return false; }
    qDebug() << "register suspend lock";
    QDBusReply<QDBusUnixFileDescriptor> reply;
    if (HasLogind() && logind && logind->isValid()) {
        reply = logind->call("Inhibit",
                             "sleep",
                             "powerkit",
                             "Lock screen etc",
                             "delay");
    }
    if (reply.isValid()) {
        suspendLock.reset(new QDBusUnixFileDescriptor(reply.value()));
        return true;
    } else {
        qWarning() << reply.error();
    }
    return false;
}

bool Manager::registerLidLock()
{
    if (lidLock) { return false; }
    qDebug() << "register lid lock";
    QDBusReply<QDBusUnixFileDescriptor> reply;
    if (HasLogind() && logind && logind->isValid()) {
        reply = logind->call("Inhibit",
                             "handle-lid-switch",
                             "powerkit",
                             "Custom lid handler",
                             "block");
    }
    if (reply.isValid()) {
        lidLock.reset(new QDBusUnixFileDescriptor(reply.value()));
        qDebug() << "lidLock" << lidLock->fileDescriptor();
        return true;
    } else {
        qWarning() << reply.error();
    }
    return false;
}

void Manager::SetWakeAlarmFromSettings()
{
    if (!CanHibernate()) { return; }
    int wmin = OnBattery()?suspendWakeupBattery:suspendWakeupAC;
    if (wmin>0) {
        qDebug() << "we need to set a wake alarm" << wmin << "min from now";
        QDateTime date = QDateTime::currentDateTime().addSecs(wmin*60);
        SetWakeAlarm(date);
    }
}

bool Manager::HasLogind()
{
    return availableService(LOGIND_SERVICE,
                            LOGIND_PATH,
                            LOGIND_MANAGER);
}

bool Manager::HasUPower()
{
    return availableService(UPOWER_SERVICE,
                            UPOWER_PATH,
                            UPOWER_MANAGER);
}

bool Manager::HasPowerKitd()
{
    return availableService(PMD_SERVICE,
                            PMD_PATH,
                            PMD_MANAGER);
}

bool Manager::HasWakeAlarm()
{
    return wakeAlarm;
}

bool Manager::HasSuspendLock()
{
    return suspendLock;
}

bool Manager::CanRestart()
{
    if (HasLogind()) {
        return availableAction(PKCanRestart, PKLogind);
    }
    return false;
}

bool Manager::CanPowerOff()
{
    if (HasLogind()) {
        return availableAction(PKCanPowerOff, PKLogind);
    }
    return false;
}

bool Manager::CanSuspend()
{
    if (HasLogind()) {
        return availableAction(PKCanSuspend, PKLogind);
    } else if (HasUPower()) {
        return availableAction(PKSuspendAllowed, PKUPower);
    }
    return false;
}

bool Manager::CanHibernate()
{
    if (HasLogind()) {
        return availableAction(PKCanHibernate, PKLogind);
    } else if (HasUPower()) {
        return availableAction(PKHibernateAllowed, PKUPower);
    }
    return false;
}

bool Manager::CanHybridSleep()
{
    if (HasLogind()) {
        return availableAction(PKCanHybridSleep, PKLogind);
    }
    return false;
}

const QString Manager::Restart()
{
    qDebug() << "try to restart";
    if (HasLogind()) {
        return executeAction(PKRestartAction, PKLogind);
    }
    return QObject::tr(PK_NO_BACKEND);
}

const QString Manager::PowerOff()
{
    qDebug() << "try to poweroff";
    if (HasLogind()) {
        return executeAction(PKPowerOffAction, PKLogind);
    }
    return QObject::tr(PK_NO_BACKEND);
}

const QString Manager::Suspend()
{
    qDebug() << "try to suspend";
    if (HasLogind()) {
        SetWakeAlarmFromSettings();
        return executeAction(PKSuspendAction, PKLogind);
    } else if (HasUPower()) {
        LockScreen();
        return executeAction(PKSuspendAction, PKUPower);
    }
    return QObject::tr(PK_NO_BACKEND);
}

const QString Manager::Hibernate()
{
    qDebug() << "try to hibernate";
    if (HasLogind()) {
        return executeAction(PKHibernateAction, PKLogind);
    } else if (HasUPower()) {
        LockScreen();
        return executeAction(PKHibernateAction, PKUPower);
    }
    return QObject::tr(PK_NO_BACKEND);
}

const QString Manager::HybridSleep()
{
    qDebug() << "try to hybridsleep";
    if (HasLogind()) {
        return executeAction(PKHybridSleepAction, PKLogind);
    }
    return QObject::tr(PK_NO_BACKEND);
}

bool Manager::SetWakeAlarm(const QDateTime &date)
{
    qDebug() << "try to set wake alarm" << date;
    if (pmd && date.isValid() && CanHibernate()) {
        if (!pmd->isValid()) { return false; }
        QDBusMessage reply = pmd->call("SetWakeAlarm",
                                       date.toString("yyyy-MM-dd HH:mm:ss"));
        const auto args = reply.arguments();
        bool alarm = args.first().toBool() && reply.errorMessage().isEmpty();
        qDebug() << "WAKE OK?" << alarm;
        wakeAlarm = alarm;
        if (alarm) {
            qDebug() << "wake alarm was set to" << date;
            wakeAlarmDate = date;
        }
        return alarm;
    }
    return false;
}

void Manager::ClearWakeAlarm()
{
    wakeAlarm = false;
}

bool Manager::IsDocked()
{
    if (logind->isValid()) { return logind->property(LOGIND_DOCKED).toBool(); }
    if (upower->isValid()) { return upower->property(UPOWER_DOCKED).toBool(); }
    return false;
}

bool Manager::LidIsPresent()
{
    if (upower->isValid()) { return upower->property(UPOWER_LID_IS_PRESENT).toBool(); }
    return false;
}

bool Manager::LidIsClosed()
{
    if (upower->isValid()) { return upower->property(UPOWER_LID_IS_CLOSED).toBool(); }
    return false;
}

bool Manager::OnBattery()
{
    if (upower->isValid()) { return upower->property(UPOWER_ON_BATTERY).toBool(); }
    return false;
}

double Manager::BatteryLeft()
{
    if (OnBattery()) { UpdateBattery(); }
    double batteryLeft = 0;
    QMapIterator<QString, Device*> device(devices);
    int batteries = 0;
    while (device.hasNext()) {
        device.next();
        if (device.value()->isBattery &&
            device.value()->isPresent &&
            !device.value()->nativePath.isEmpty())
        {
            batteryLeft += device.value()->percentage;
            batteries++;
        } else { continue; }
    }
    return batteryLeft/batteries;
}

void Manager::LockScreen()
{
    qDebug() << "screen lock";
    QProcess::startDetached(Settings::getValue(CONF_SCREENSAVER_LOCK_CMD,
                                               PK_SCREENSAVER_LOCK_CMD).toString(),
                            QStringList());
}

bool Manager::HasBattery()
{
    QMapIterator<QString, Device*> device(devices);
    while (device.hasNext()) {
        device.next();
        if (device.value()->isBattery) { return true; }
    }
    return false;
}

qlonglong Manager::TimeToEmpty()
{
    if (OnBattery()) { UpdateBattery(); }
    qlonglong result = 0;
    QMapIterator<QString, Device*> device(devices);
    while (device.hasNext()) {
        device.next();
        if (device.value()->isBattery &&
            device.value()->isPresent &&
            !device.value()->nativePath.isEmpty())
        { result += device.value()->timeToEmpty; }
    }
    return result;
}

qlonglong Manager::TimeToFull()
{
    if (OnBattery()) { UpdateBattery(); }
    qlonglong result = 0;
    QMapIterator<QString, Device*> device(devices);
    while (device.hasNext()) {
        device.next();
        if (device.value()->isBattery &&
            device.value()->isPresent &&
            !device.value()->nativePath.isEmpty())
        { result += device.value()->timeToFull; }
    }
    return result;
}

void Manager::UpdateDevices()
{
    QMapIterator<QString, Device*> device(devices);
    while (device.hasNext()) {
        device.next();
        device.value()->update();
    }
}

void Manager::UpdateBattery()
{
    QMapIterator<QString, Device*> device(devices);
    while (device.hasNext()) {
        device.next();
        if (device.value()->isBattery) {
            device.value()->updateBattery();
        }
    }
}

void Manager::UpdateConfig()
{
    emit Update();
}

const QStringList Manager::GetScreenSaverInhibitors()
{
    QStringList result;
    QMapIterator<quint32, QString> i(ssInhibitors);
    while (i.hasNext()) {
        i.next();
        result << i.value();
    }
    return result;
}

const QStringList Manager::GetPowerManagementInhibitors()
{
    QStringList result;
    QMapIterator<quint32, QString> i(pmInhibitors);
    while (i.hasNext()) {
        i.next();
        result << i.value();
    }
    return result;
}

QMap<quint32, QString> Manager::GetInhibitors()
{
    QMap<quint32, QString> result;
    QMapIterator<quint32, QString> ss(ssInhibitors);
    while (ss.hasNext()) {
        ss.next();
        result.insert(ss.key(), ss.value());
    }
    QMapIterator<quint32, QString> pm(pmInhibitors);
    while (pm.hasNext()) {
        pm.next();
        result.insert(pm.key(), pm.value());
    }
    return result;
}

const QDateTime Manager::GetWakeAlarm()
{
    return wakeAlarmDate;
}

void Manager::ReleaseSuspendLock()
{
    qDebug() << "release suspend lock";
    suspendLock.reset(nullptr);
}

void Manager::ReleaseLidLock()
{
    qDebug() << "release lid lock";
    lidLock.reset(nullptr);
}

void Manager::SetSuspendWakeAlarmOnBattery(int value)
{
    qDebug() << "set suspend wake alarm on battery" << value;
    suspendWakeupBattery = value;
}

void Manager::SetSuspendWakeAlarmOnAC(int value)
{
    qDebug() << "set suspend wake alarm on ac" << value;
    suspendWakeupAC = value;
}

bool Manager::SetDisplayBacklight(const QString &device, int value)
{
    qDebug() << "PK SET DISPLAY BACKLIGHT" << device << value;
    if (!pmd) { return false; }
    if (!pmd->isValid()) { return false; }
    QDBusMessage reply = pmd->call("SetDisplayBacklight",
                                   device,
                                   value);
    const auto args = reply.arguments();
    bool backlight = args.first().toBool() && reply.errorMessage().isEmpty();
    qDebug() << "BACKLIGHT OK?" << backlight << reply.errorMessage();
    return backlight;
}

bool Manager::SetPState(int min, int max)
{
    qDebug() << "PK SET PSTATE" << min << max;
    if (!pmd) { return false; }
    if (!pmd->isValid()) { return false; }
    QDBusMessage reply = pmd->call("SetPState",
                                   min,
                                   max);
    const auto args = reply.arguments();
    bool pstate = args.first().toBool() && reply.errorMessage().isEmpty();
    qDebug() << "PSTATE OK?" << pstate << reply.errorMessage();
    return pstate;
}

bool Manager::SetPStateMin(int value)
{
    qDebug() << "PK SET PSTATE" << value;
    if (!pmd) { return false; }
    if (!pmd->isValid()) { return false; }
    QDBusMessage reply = pmd->call("SetPStateMin", value);
    const auto args = reply.arguments();
    bool pstate = args.first().toBool() && reply.errorMessage().isEmpty();
    qDebug() << "PSTATE OK?" << pstate << reply.errorMessage();
    return pstate;
}

bool Manager::SetPStateMax(int value)
{
    qDebug() << "PK SET PSTATE" << value;
    if (!pmd) { return false; }
    if (!pmd->isValid()) { return false; }
    QDBusMessage reply = pmd->call("SetPStateMax", value);
    const auto args = reply.arguments();
    bool pstate = args.first().toBool() && reply.errorMessage().isEmpty();
    qDebug() << "PSTATE OK?" << pstate << reply.errorMessage();
    return pstate;
}
