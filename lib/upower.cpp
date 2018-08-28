/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
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

bool UPower::hasService()
{
    QDBusInterface iface(UP_SERVICE,
                         UP_PATH,
                         UP_SERVICE,
                         QDBusConnection::systemBus());
    if (iface.isValid()) { return true; }
    return false;
}

bool UPower::canSuspend()
{
    QDBusInterface iface(UP_SERVICE, UP_PATH, UP_SERVICE, QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    //QDBusMessage reply = iface.call("CanSuspend");
    QDBusMessage reply = iface.call("SuspendAllowed");
    bool result = reply.arguments().first().toBool();
    if (!reply.errorMessage().isEmpty()) { result = false; }
    qDebug() << "can suspend?" << result << reply;
    return result;
}

QString UPower::suspend()
{
    QDBusInterface iface(UP_SERVICE, UP_PATH, UP_SERVICE, QDBusConnection::systemBus());
    if (!iface.isValid()) { return QObject::tr("Failed D-Bus connection."); }
    QDBusMessage reply = iface.call("Suspend");
    return reply.errorMessage();
}

bool UPower::canHibernate()
{
    QDBusInterface iface(UP_SERVICE, UP_PATH, UP_SERVICE, QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    //QDBusMessage reply = iface.call("CanHibernate");
    QDBusMessage reply = iface.call("HibernateAllowed");
    bool result = reply.arguments().first().toBool();
    if (!reply.errorMessage().isEmpty()) { result = false; }
    qDebug() << "can hibernate?" << result << reply.arguments();
    return result;
}

QString UPower::hibernate()
{
    QDBusInterface iface(UP_SERVICE, UP_PATH, UP_SERVICE, QDBusConnection::systemBus());
    if (!iface.isValid()) { return QObject::tr("Failed D-Bus connection."); }
    QDBusMessage reply = iface.call("Hibernate");
    return reply.errorMessage();
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
