/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit_power_device.h"

#include <QDBusConnection>
#include <QStringList>

#define PROP_CHANGED "PropertiesChanged"
#define PROP_DEV_MODEL "Model"
#define PROP_DEV_CAPACITY "Capacity"
#define PROP_DEV_IS_RECHARGE "IsRechargeable"
#define PROP_DEV_PRESENT "IsPresent"
#define PROP_DEV_PERCENT "Percentage"
#define PROP_DEV_ENERGY_FULL_DESIGN "EnergyFullDesign"
#define PROP_DEV_ENERGY_FULL "EnergyFull"
#define PROP_DEV_ENERGY_EMPTY "EnergyEmpty"
#define PROP_DEV_ENERGY "Energy"
#define PROP_DEV_ONLINE "Online"
#define PROP_DEV_POWER_SUPPLY "PowerSupply"
#define PROP_DEV_TIME_TO_EMPTY "TimeToEmpty"
#define PROP_DEV_TIME_TO_FULL "TimeToFull"
#define PROP_DEV_TYPE "Type"
#define PROP_DEV_VENDOR "Vendor"
#define PROP_DEV_NATIVEPATH "NativePath"

Device::Device(const QString block, QObject *parent)
    : QObject(parent)
    , path(block)
    , isRechargable(false)
    , isPresent(false)
    , percentage(0)
    , online(false)
    , hasPowerSupply(false)
    , isBattery(false)
    , isAC(false)
    , capacity(0)
    , energy(0)
    , energyFullDesign(0)
    , energyFull(0)
    , energyEmpty(0)
    , dbus(nullptr)
    , dbusp(nullptr)
{
    QDBusConnection system = QDBusConnection::systemBus();
    dbus = new QDBusInterface(UPOWER_SERVICE,
                              path,
                              QString("%1.%2").arg(UPOWER_SERVICE).arg(DBUS_DEVICE),
                              system,
                              parent);
    system.connect(dbus->service(),
                   dbus->path(),
                   QString("%1.%2").arg(UPOWER_SERVICE).arg(DBUS_DEVICE),
                   DBUS_CHANGED,
                   this,
                   SLOT(updateDeviceProperties()));
    dbusp = new QDBusInterface(UPOWER_SERVICE,
                               path,
                               DBUS_PROPERTIES,
                               system,
                               parent);
    system.connect(dbusp->service(),
                   dbusp->path(),
                   DBUS_PROPERTIES,
                   PROP_CHANGED,
                   this,
                   SLOT(updateDeviceProperties()));
    if (name.isEmpty()) { name = path.split("/").takeLast(); }
    updateDeviceProperties();
}

// get device properties
void Device::updateDeviceProperties()
{
    if (!dbus->isValid()) { return; }

    model = dbus->property(PROP_DEV_MODEL).toString();
    capacity =  dbus->property(PROP_DEV_CAPACITY).toDouble();
    isRechargable =  dbus->property(PROP_DEV_IS_RECHARGE).toBool();
    isPresent =  dbus->property(PROP_DEV_PRESENT).toBool();
    percentage =  dbus->property(PROP_DEV_PERCENT).toDouble();
    energyFullDesign = dbus->property(PROP_DEV_ENERGY_FULL_DESIGN).toDouble();
    energyFull = dbus->property(PROP_DEV_ENERGY_FULL).toDouble();
    energyEmpty = dbus->property(PROP_DEV_ENERGY_EMPTY).toDouble();
    energy = dbus->property(PROP_DEV_ENERGY).toDouble();
    online = dbus->property(PROP_DEV_ONLINE).toBool();
    hasPowerSupply = dbus->property(PROP_DEV_POWER_SUPPLY).toBool();
    timeToEmpty = dbus->property(PROP_DEV_TIME_TO_EMPTY).toLongLong();
    timeToFull = dbus->property(PROP_DEV_TIME_TO_FULL).toLongLong();
    type = (DeviceType)dbus->property(PROP_DEV_TYPE).toUInt();

    if (type == DeviceBattery) { isBattery = true; }
    else {
        isBattery = false;
        if (type == DeviceLinePower) { isAC = true; }
        else { isAC = false; }
    }

    vendor = dbus->property(PROP_DEV_VENDOR).toString();
    nativePath = dbus->property(PROP_DEV_NATIVEPATH).toString();

    emit deviceChanged(path);
}

void Device::update()
{
    updateDeviceProperties();
}

void Device::updateBattery()
{
    percentage =  dbus->property(PROP_DEV_PERCENT).toDouble();
    timeToEmpty = dbus->property(PROP_DEV_TIME_TO_EMPTY).toLongLong();
    timeToFull = dbus->property(PROP_DEV_TIME_TO_FULL).toLongLong();
}
