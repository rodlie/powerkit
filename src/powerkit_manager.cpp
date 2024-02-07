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
#define PK_CAN_SUSPEND_THEN_HIBERNATE "CanSuspendThenHibernate"
#define PK_SUSPEND_THEN_HIBERNATE "SuspendThenHibernate"
#define PK_NO_BACKEND "No backend available."
#define PK_NO_ACTION "Action no available."

#define TIMEOUT_CHECK 60000

using namespace PowerKit;

Manager::Manager(QObject *parent) : QObject(parent)
  , upower(nullptr)
  , logind(nullptr)
  , wasDocked(false)
  , wasLidClosed(false)
  , wasOnBattery(false)
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

bool Manager::canLogind(const QString &method)
{
    if (!logind->isValid() || method.isEmpty()) { return false; }
    QDBusMessage reply = logind->call(method);
    const auto args = reply.arguments();
    if (args.first().toString() == DBUS_OK_REPLY) { return true; }
    bool result = args.first().toBool();
    if (!reply.errorMessage().isEmpty()) {
        result = false;
        emit Warning(reply.errorMessage());
    }
    return result;
}

const QDBusMessage Manager::callLogind(const QString &method)
{
    QDBusMessage reply;
    if (logind->isValid() && !method.isEmpty()) {
        reply = logind->call(method, true);
    }
    return reply;
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
        emit Warning(tr("Find devices failed, check the upower service!"));
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
    if (!system.isConnected()) {
        emit Error(tr("Failed to connect to the system bus"));
        return;
    }

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
    system.connect(UPOWER_SERVICE,
                   UPOWER_PATH,
                   DBUS_PROPERTIES,
                   DBUS_PROPERTIES_CHANGED,
                   this,
                   SLOT(propertiesChanged()));

    upower = new QDBusInterface(UPOWER_SERVICE,
                                UPOWER_PATH,
                                UPOWER_MANAGER,
                                system,
                                this);
    logind = new QDBusInterface(LOGIND_SERVICE,
                                LOGIND_PATH,
                                LOGIND_MANAGER,
                                system,
                                this);

    if (!upower->isValid()) {
        emit Error(tr("Failed to connect to upower"));
        return;
    }
    if (!logind->isValid()) {
        emit Error(tr("Failed to connect to logind"));
        return;
    }

    wasDocked = IsDocked();
    wasLidClosed = LidIsClosed();
    wasOnBattery = OnBattery();

    connect(logind,
            SIGNAL(PrepareForSleep(bool)),
            this,
            SLOT(handlePrepareForSuspend(bool)));

    if (!suspendLock) { registerSuspendLock(); }
    if (!lidLock) { registerLidLock(); }

    scan();
}

void Manager::check()
{
    if (!suspendLock) { registerSuspendLock(); }
    if (!lidLock) { registerLidLock(); }
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
            qDebug() << "lid changed to closed";
            emit LidClosed();
        } else if (wasLidClosed && !isLidClosed) {
            qDebug() << "lid changed to open";
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
    wasOnBattery = isOnBattery;

    deviceChanged();
}

void Manager::handleDeviceChanged(const QString &device)
{
    Q_UNUSED(device)
    qDebug() << "device changed" << device;
    deviceChanged();
}

void Manager::handlePrepareForSuspend(bool prepare)
{
    qDebug() << "handle prepare for suspend/resume" << prepare;
    if (prepare) {
        qDebug() << "ZZZ";
        LockScreen();
        emit PrepareForSuspend();
        QTimer::singleShot(500, this, SLOT(ReleaseSuspendLock()));
    }
    else {
        qDebug() << "WAKE UP!";
        UpdateDevices();
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

void Manager::handleNewInhibitScreenSaver(const QString &application,
                                          const QString &reason,
                                          quint32 cookie)
{
    qDebug() << "new screensaver cookie" << application << reason << cookie;
    Q_UNUSED(reason)
    ssInhibitors[cookie] = application;
    emit UpdatedInhibitors();
}

void Manager::handleNewInhibitPowerManagement(const QString &application,
                                              const QString &reason,
                                              quint32 cookie)
{
    qDebug() << "new power cookie" << application << reason << cookie;
    Q_UNUSED(reason)
    pmInhibitors[cookie] = application;
    emit UpdatedInhibitors();
}

void Manager::handleDelInhibitScreenSaver(quint32 cookie)
{
    qDebug() << "remove screensaver cookie" << cookie;
    if (ssInhibitors.contains(cookie)) {
        ssInhibitors.remove(cookie);
        emit UpdatedInhibitors();
    }
}

void Manager::handleDelInhibitPowerManagement(quint32 cookie)
{
    qDebug() << "remove power cookie" << cookie;
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
    if (logind->isValid()) {
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
        emit Warning(tr("Failed to set suspend lock: %1").arg(reply.error().message()));
    }
    return false;
}

bool Manager::registerLidLock()
{
    if (lidLock) { return false; }
    qDebug() << "register lid lock";
    QDBusReply<QDBusUnixFileDescriptor> reply;
    if (logind->isValid()) {
        reply = logind->call("Inhibit",
                             "handle-lid-switch",
                             "powerkit",
                             "Custom lid handler",
                             "block");
    }
    if (reply.isValid()) {
        lidLock.reset(new QDBusUnixFileDescriptor(reply.value()));
        return true;
    } else {
        emit Warning(tr("Failed to set lid lock: %1").arg(reply.error().message()));
    }
    return false;
}

bool Manager::HasSuspendLock()
{
    return suspendLock;
}

bool Manager::HasLidLock()
{
    return lidLock;
}

bool Manager::CanRestart()
{
    if (logind->isValid()) { return canLogind(PK_CAN_RESTART); }
    return false;
}

bool Manager::CanPowerOff()
{
    if (logind->isValid()) { return canLogind(PK_CAN_POWEROFF); }
    return false;
}

bool Manager::CanSuspend()
{
    if (logind->isValid()) { return canLogind(PK_CAN_SUSPEND); }
    return false;
}

bool Manager::CanHibernate()
{
    if (logind->isValid()) { return canLogind(PK_CAN_HIBERNATE); }
    return false;
}

bool Manager::CanHybridSleep()
{
    if (logind->isValid()) { return canLogind(PK_CAN_HYBRIDSLEEP); }
    return false;
}

bool Manager::CanSuspendThenHibernate()
{
    if (logind->isValid()) { return canLogind(PK_CAN_SUSPEND_THEN_HIBERNATE); }
    return false;
}

const QString Manager::Restart()
{
    const auto reply = callLogind(PK_RESTART);
    qDebug() << "restart reply" << reply;
    return reply.errorMessage();
}

const QString Manager::PowerOff()
{
    const auto reply = callLogind(PK_POWEROFF);
    qDebug() << "poweroff reply" << reply;
    return reply.errorMessage();
}

const QString Manager::Suspend()
{
    const auto reply = callLogind(PK_SUSPEND);
    qDebug() << "suspend reply" << reply;
    return reply.errorMessage();
}

const QString Manager::Hibernate()
{
    const auto reply = callLogind(PK_HIBERNATE);
    qDebug() << "hibernate reply" << reply;
    return reply.errorMessage();
}

const QString Manager::HybridSleep()
{
    const auto reply = callLogind(PK_HYBRIDSLEEP);
    qDebug() << "hybridsleep reply" << reply;
    return reply.errorMessage();
}

const QString Manager::SuspendThenHibernate()
{
    const auto reply = callLogind(PK_SUSPEND_THEN_HIBERNATE);
    qDebug() << "suspend then hibernate reply" << reply;
    return reply.errorMessage();
}

bool Manager::IsDocked()
{
    if (logind->isValid()) { return logind->property(LOGIND_DOCKED).toBool(); }
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
