/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "power.h"
#include "upower.h"
#include <QTimer>
#include <QProcess>

#include "def.h"
#include "login1.h"
#include "ckit.h"

Power::Power(QObject *parent)
    : QObject(parent)
    , upower(0)
    , logind(0)
    , wasDocked(false)
    , wasLidClosed(false)
    , wasOnBattery(false)
{
    // setup dbus connection and start timer
    setupDBus();
    timer.setInterval(CHECK_UPOWER_TIMEOUT);
    connect(&timer,
            SIGNAL(timeout()),
            this,
            SLOT(checkUPower()));
    timer.start();
}

// get dbus properties
bool Power::isDocked()
{
    if (logind->isValid()) { return logind->property(PROP_LOGIN1_DOCKED).toBool(); }
    if (upower->isValid()) { return upower->property(PROP_UPOWER_DOCKED).toBool(); }
    return false;
}

bool Power::lidIsPresent()
{
    if (upower->isValid()) { return upower->property(PROP_UPOWER_LID_IS_PRESENT).toBool(); }
    return false;
}

bool Power::lidIsClosed()
{
    if (upower->isValid()) { return upower->property(PROP_UPOWER_LID_IS_CLOSED).toBool(); }
    return false;
}

bool Power::onBattery()
{
    if (upower->isValid()) { return upower->property(PROP_UPOWER_ON_BATTERY).toBool(); }
    return false;
}

bool Power::canHibernate()
{
    if (logind->isValid()) { return Login1::canHibernate(); }
    if (upower->isValid()) { return upower->property(PROP_UPOWER_CAN_HIBERNATE).toBool(); }
    return false;
}

bool Power::canSuspend()
{
    if (logind->isValid()) { return Login1::canSuspend(); }
    if (upower->isValid()) { return upower->property(PROP_UPOWER_CAN_SUSPEND).toBool(); }
    return false;
}

// get total battery left
double Power::batteryLeft()
{
    if (onBattery()) { updateBattery(); }
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

// do suspend if available
void Power::sleep()
{
    if (canSuspend()) {
        if (logind->isValid()) {
            Login1::suspend();
            return;
        }
        if (upower->isValid()) { UPower::suspend(); }
    }
}

// do hibernate if available
void Power::hibernate()
{
    if (canHibernate()) {
        if (logind->isValid()) {
            Login1::hibernate();
            return;
        }
        if (upower->isValid()) { UPower::hibernate(); }
    }
}

// lock screen using xscreensaver
void Power::lockScreen()
{
    QProcess proc;
    proc.start(XSCREENSAVER_LOCK);
    proc.waitForFinished();
    proc.close();
}

void Power::shutdown()
{
    if (logind->isValid()) {
        if (Login1::canPowerOff()) {
            Login1::poweroff();
            return;
        }
    }
    if (upower->isValid()) {
        if (CKit::canPowerOff()) { CKit::poweroff(); }
    }
}

bool Power::hasBattery()
{
    QMapIterator<QString, Device*> device(devices);
    while (device.hasNext()) {
        device.next();
        if (device.value()->isBattery) { return true; }
    }
    return false;
}

qlonglong Power::timeToEmpty()
{
    if (onBattery()) { updateBattery(); }
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

qlonglong Power::timeToFull()
{
    if (onBattery()) { updateBattery(); }
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

void Power::update()
{
    QMapIterator<QString, Device*> device(devices);
    while (device.hasNext()) {
        device.next();
        device.value()->update();
    }
}

void Power::updateBattery()
{
    QMapIterator<QString, Device*> device(devices);
    while (device.hasNext()) {
        device.next();
        if (device.value()->isBattery) {
            device.value()->updateBattery();
        }
    }
}

// setup dbus connections
void Power::setupDBus()
{
    QDBusConnection system = QDBusConnection::systemBus();
    if (system.isConnected()) {
        system.connect(UP_SERVICE,
                       UP_PATH,
                       UP_SERVICE,
                       DBUS_DEVICE_ADDED,
                       this,
                       SLOT(deviceAdded(const QDBusObjectPath&)));
        system.connect(UP_SERVICE,
                       UP_PATH,
                       UP_SERVICE,
                       DBUS_DEVICE_ADDED,
                       this,
                       SLOT(deviceAdded(const QString&)));
        system.connect(UP_SERVICE,
                       UP_PATH,
                       UP_SERVICE,
                       DBUS_DEVICE_REMOVED,
                       this,
                       SLOT(deviceRemoved(const QDBusObjectPath&)));
        system.connect(UP_SERVICE,
                       UP_PATH,
                       UP_SERVICE,
                       DBUS_DEVICE_REMOVED,
                       this,
                       SLOT(deviceRemoved(const QString&)));
        system.connect(UP_SERVICE,
                       UP_PATH,
                       UP_SERVICE,
                       UP_CONN_CHANGED,
                       this,
                       SLOT(deviceChanged()));
        system.connect(UP_SERVICE,
                       UP_PATH,
                       UP_SERVICE,
                       UP_CONN_DEV_CHANGED,
                       this,
                       SLOT(deviceChanged()));
        system.connect(UP_SERVICE,
                       UP_PATH,
                       UP_SERVICE,
                       UP_CONN_NOTIFY_RESUME,
                       this,
                       SLOT(notifyResume()));
        system.connect(UP_SERVICE,
                       UP_PATH,
                       UP_SERVICE,
                       UP_CONN_NOTIFY_SLEEP,
                       this,
                       SLOT(notifySleep()));
        system.connect(LOGIN1_SERVICE,
                       LOGIN1_PATH,
                       LOGIN1_MANAGER,
                       LOGIN1_PREP_FOR_SLEEP,
                       this,
                       SLOT(notifySuspend(bool)));
        if (upower == NULL) {
            upower = new QDBusInterface(UP_SERVICE,
                                        UP_PATH,
                                        UP_SERVICE,
                                        system);
        }
        if (logind == NULL) {
            logind = new QDBusInterface(LOGIN1_SERVICE,
                                        LOGIN1_PATH,
                                        LOGIN1_MANAGER,
                                        system);
        }
        scanDevices();
    }
}

// scan for new devices
void Power::scanDevices()
{
    QStringList foundDevices = UPower::getDevices();
    for (int i=0; i < foundDevices.size(); i++) {
        QString foundDevicePath = foundDevices.at(i);
        bool hasDevice = devices.contains(foundDevicePath);
        if (hasDevice) { continue; }
        Device *newDevice = new Device(foundDevicePath, this);
        connect(newDevice,
                SIGNAL(deviceChanged(QString)),
                this,
                SLOT(handleDeviceChanged(QString)));
        devices[foundDevicePath] = newDevice;
    }
    update();
    emit updatedDevices();
}

// add device if not exists
void Power::deviceAdded(const QDBusObjectPath &obj)
{
    deviceAdded(obj.path());
}

void Power::deviceAdded(const QString &path)
{
    if (!upower->isValid()) { return; }
    if (path.startsWith(QString(DBUS_JOBS).arg(UP_PATH))) { return; }
    emit deviceWasAdded(path);
    scanDevices();
}

// remove device if exists
void Power::deviceRemoved(const QDBusObjectPath &obj)
{
    deviceRemoved(obj.path());
}

void Power::deviceRemoved(const QString &path)
{
    if (!upower->isValid()) { return; }
    bool deviceExists = devices.contains(path);
    if (path.startsWith(QString(DBUS_JOBS).arg(UP_PATH))) { return; }
    if (deviceExists) {
        if (UPower::getDevices().contains(path)) { return; }
        delete devices.take(path);
        emit deviceWasRemoved(path);
    }
    scanDevices();
}

// check device status when changed
void Power::deviceChanged()
{
    if (wasLidClosed != lidIsClosed()) {
        if (!wasLidClosed && lidIsClosed()) {
            emit closedLid();
        } else if (wasLidClosed && !lidIsClosed()) {
            emit openedLid();
        }
    }
    wasLidClosed = lidIsClosed();

    if (wasOnBattery != onBattery()) {
        if (!wasOnBattery && onBattery()) {
            emit switchedToBattery();
        } else if (wasOnBattery && !onBattery()) {
            emit switchedToAC();
        }
    }
    wasOnBattery = onBattery();

    emit updatedDevices();
}

// handle device changes
void Power::handleDeviceChanged(QString devicePath)
{
    if (devicePath.isEmpty()) { return; }
    deviceChanged();
}

// check if dbus is connected, if not connect
void Power::checkUPower()
{
    if (!QDBusConnection::systemBus().isConnected()) {
        setupDBus();
        return;
    }
    if (!upower->isValid()) { scanDevices(); }
}

// do stuff when resuming
void Power::notifyResume()
{
    emit aboutToResume();
}

// do stuff before sleep
void Power::notifySleep()
{
    emit aboutToSuspend();
}

void Power::notifySuspend(bool suspend)
{
    if (suspend) { emit aboutToSuspend(); }
    else { emit aboutToResume();}
}
