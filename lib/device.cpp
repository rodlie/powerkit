/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "device.h"

#include <QDBusConnection>
#include <QStringList>
#include <QDebug>

#include "def.h"

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
    , dbus(0)
    , dbusp(0)
{
    QDBusConnection system = QDBusConnection::systemBus();
    dbus = new QDBusInterface(UP_SERVICE,
                              path,
                              QString("%1.Device").arg(UP_SERVICE),
                              system,
                              parent);
    system.connect(dbus->service(),
                   dbus->path(),
                   QString("%1.Device").arg(UP_SERVICE),
                   "Changed",
                   this,
                   SLOT(updateDeviceProperties()));
    dbusp = new QDBusInterface(UP_SERVICE,
                               path,
                               DBUS_PROPERTIES,
                               system,
                               parent);
    system.connect(dbusp->service(),
                   dbusp->path(),
                   DBUS_PROPERTIES,
                   "PropertiesChanged",
                   this,
                   SLOT(updateDeviceProperties()));
    if (name.isEmpty()) { name = path.split("/").takeLast(); }
    updateDeviceProperties();
}

// get device properties
void Device::updateDeviceProperties()
{
    if (!dbus->isValid()) { return; }
    qDebug() << "device changed!" << path;
    model = dbus->property("Model").toString();
    capacity =  dbus->property("Capacity").toDouble();
    isRechargable =  dbus->property("IsRechargeable").toBool();
    isPresent =  dbus->property("IsPresent").toBool();
    percentage =  dbus->property("Percentage").toDouble();
    energyFullDesign = dbus->property("EnergyFullDesign").toDouble();
    energyFull = dbus->property("EnergyFull").toDouble();
    energyEmpty = dbus->property("EnergyEmpty").toDouble();
    energy = dbus->property("Energy").toDouble();
    online = dbus->property("Online").toBool();
    hasPowerSupply = dbus->property("PowerSupply").toBool();
    timeToEmpty = dbus->property("TimeToEmpty").toLongLong();
    timeToFull = dbus->property("TimeToFull").toLongLong();
    type = (DeviceType)dbus->property("Type").toUInt();

    if (type == DeviceBattery) { isBattery = true; }
    else {
        isBattery = false;
        if (type == DeviceLinePower) { isAC = true; }
        else { isAC = false; }
    }

    vendor = dbus->property("Vendor").toString();
    nativePath = dbus->property("NativePath").toString();

    emit deviceChanged(path);
}
