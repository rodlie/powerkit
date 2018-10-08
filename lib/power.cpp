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
#include <QDebug>

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
    timer.setInterval(60000);
    connect(&timer,
            SIGNAL(timeout()),
            this,
            SLOT(checkUPower()));
    timer.start();
}

// get dbus properties
bool Power::isDocked()
{
    if (logind->isValid()) { return logind->property("Docked").toBool(); }
    if (upower->isValid()) { return upower->property("IsDocked").toBool(); }
    return false;
}

bool Power::lidIsPresent()
{
    if (upower->isValid()) { return upower->property("LidIsPresent").toBool(); }
    return false;
}

bool Power::lidIsClosed()
{
    if (upower->isValid()) { return upower->property("LidIsClosed").toBool(); }
    return false;
}

bool Power::onBattery()
{
    if (upower->isValid()) { return upower->property("OnBattery").toBool(); }
    return false;
}

bool Power::canHibernate()
{
    if (logind->isValid()) { return Login1::canHibernate(); }
    if (upower->isValid()) { return upower->property("CanHibernate").toBool(); }
    return false;
}

bool Power::canSuspend()
{
    if (logind->isValid()) { return Login1::canSuspend(); }
    if (upower->isValid()) { return upower->property("CanSuspend").toBool(); }
    return false;
}

// get total battery left
double Power::batteryLeft()
{
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

// setup dbus connections
void Power::setupDBus()
{
    QDBusConnection system = QDBusConnection::systemBus();
    if (system.isConnected()) {
        qDebug() << "dbus is running";
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
                       "Changed",
                       this,
                       SLOT(deviceChanged()));
        system.connect(UP_SERVICE,
                       UP_PATH,
                       UP_SERVICE,
                       "DeviceChanged",
                       this,
                       SLOT(deviceChanged()));
        system.connect(UP_SERVICE,
                       UP_PATH,
                       UP_SERVICE,
                       "NotifyResume",
                       this,
                       SLOT(notifyResume()));
        // missing notify resume on logind!!!
        system.connect(UP_SERVICE,
                       UP_PATH,
                       UP_SERVICE,
                       "NotifySleep",
                       this,
                       SLOT(notifySleep()));
        system.connect(LOGIN1_SERVICE,
                       LOGIN1_PATH,
                       LOGIN1_MANAGER,
                       "PrepareForSleep",
                       this,
                       SLOT(notifySleep()));
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
    qDebug() << "scan devices";
    QStringList foundDevices = UPower::getDevices();
    for (int i=0; i < foundDevices.size(); i++) {
        QString foundDevicePath = foundDevices.at(i);
        bool hasDevice = devices.contains(foundDevicePath);
        if (hasDevice) { continue; }
        qDebug() << "found new device" << foundDevicePath;
        Device *newDevice = new Device(foundDevicePath, this);
        connect(newDevice,
                SIGNAL(deviceChanged(QString)),
                this,
                SLOT(handleDeviceChanged(QString)));
        devices[foundDevicePath] = newDevice;
    }
    emit updatedDevices();
}

// add device if not exists
void Power::deviceAdded(const QDBusObjectPath &obj)
{
    deviceAdded(obj.path());
}

void Power::deviceAdded(const QString &path)
{
    qDebug() << "deviceAdded" << path;
    if (!upower->isValid()) { return; }
    if (path.startsWith(QString("%1/jobs").arg(UP_PATH))) { return; }
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
    qDebug() << "deviceRemoved" << path;
    if (!upower->isValid()) { return; }
    bool deviceExists = devices.contains(path);
    if (path.startsWith(QString("%1/jobs").arg(UP_PATH))) { return; }
    if (deviceExists) {
        qDebug() << "remove device" << path;
        if (UPower::getDevices().contains(path)) { return; }
        delete devices.take(path);
        emit deviceWasRemoved(path);
    }
    scanDevices();
}

// check device status when changed
void Power::deviceChanged()
{
    qDebug() << "device changed, check lid and battery status";
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
    qDebug() << "device changed?" << devicePath;
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
// this does not work on newer upower/logind
void Power::notifyResume()
{
    qDebug() << "notifyResume";
    scanDevices();
    emit aboutToResume();
    //lockScreen();
}

// do stuff before sleep
void Power::notifySleep()
{
    qDebug() << "notifySleep";
    emit aboutToSuspend();
    emit notifyStatus(tr("About to suspend"), tr("System is about to suspend ..."));
    lockScreen();
}
