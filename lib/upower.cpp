/*
# PowerDwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#
*/

#include "upower.h"
#include <QDBusObjectPath>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusInterface>
#include <QXmlStreamReader>
#include <QDebug>
#include <QStringList>

#include "def.h"

bool UPower::canPowerOff()
{
    QDBusInterface iface(CKIT_SERVICE, CKIT_PATH, CKIT_MANAGER, QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    QDBusMessage reply = iface.call("CanPowerOff");
    return reply.arguments().first().toBool();
}

QString UPower::poweroff()
{
    QDBusInterface iface(CKIT_SERVICE, CKIT_PATH, CKIT_MANAGER, QDBusConnection::systemBus());
    if (!iface.isValid()) { return QObject::tr("Failed D-Bus connection."); }
    QDBusMessage reply = iface.call("PowerOff");
    return reply.arguments().first().toString();
}

bool UPower::canSuspend()
{
    QDBusInterface iface(UP_SERVICE, UP_PATH, UP_SERVICE, QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    QDBusMessage reply = iface.call("CanSuspend");
    return reply.arguments().first().toBool();
}

QString UPower::suspend()
{
    QDBusInterface iface(UP_SERVICE, UP_PATH, UP_SERVICE, QDBusConnection::systemBus());
    if (!iface.isValid()) { return QObject::tr("Failed D-Bus connection."); }
    QDBusMessage reply = iface.call("Suspend");
    return reply.arguments().first().toString();
}

bool UPower::canHibernate()
{
    QDBusInterface iface(UP_SERVICE, UP_PATH, UP_SERVICE, QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    QDBusMessage reply = iface.call("CanHibernate");
    return reply.arguments().first().toBool();
}

QString UPower::hibernate()
{
    QDBusInterface iface(UP_SERVICE, UP_PATH, UP_SERVICE, QDBusConnection::systemBus());
    if (!iface.isValid()) { return QObject::tr("Failed D-Bus connection."); }
    QDBusMessage reply = iface.call("Hibernate");
    return reply.arguments().first().toString();
}

QStringList UPower::getDevices()
{
    QStringList result;
    QDBusMessage call = QDBusMessage::createMethodCall(UP_SERVICE, QString("%1/devices").arg(UP_PATH), DBUS_INTROSPECTABLE, "Introspect");
    QDBusPendingReply<QString> reply = QDBusConnection::systemBus().call(call);
    QList<QDBusObjectPath> devices;
    QXmlStreamReader xml(reply.value());
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name().toString() == "node" ) {
            QString name = xml.attributes().value("name").toString();
            if(!name.isEmpty()) { devices << QDBusObjectPath("/org/freedesktop/UPower/devices/" + name); }
        }
    }
    foreach (QDBusObjectPath device, devices) {
        result << device.path();
    }
    return result;
}
