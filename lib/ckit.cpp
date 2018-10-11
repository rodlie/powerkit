/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "ckit.h"
#include "def.h"

#include <QDBusInterface>
#include <QDBusMessage>

bool CKit::hasService()
{
    QDBusInterface iface(CKIT_SERVICE,
                         CKIT_PATH,
                         CKIT_MANAGER,
                         QDBusConnection::systemBus());
    if (iface.isValid()) { return true; }
    return false;
}

bool CKit::canRestart()
{
    QDBusInterface iface(CKIT_SERVICE, CKIT_PATH, CKIT_MANAGER,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    QDBusMessage reply = iface.call(CKIT_CAN_RESTART);
    if (reply.arguments().first().toString() == "yes") { return true; }
    return false;
}

QString CKit::restart()
{
    QDBusInterface iface(CKIT_SERVICE, CKIT_PATH, CKIT_MANAGER,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return QObject::tr(DBUS_FAILED_CONN); }
    QDBusMessage reply = iface.call(CKIT_RESTART, true);
    return reply.errorMessage();
}

bool CKit::canPowerOff()
{
    QDBusInterface iface(CKIT_SERVICE, CKIT_PATH, CKIT_MANAGER,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    QDBusMessage reply = iface.call(CKIT_CAN_POWEROFF);
    if (reply.arguments().first().toString() == "yes") { return true; }
    return false;
}

QString CKit::poweroff()
{
    QDBusInterface iface(CKIT_SERVICE, CKIT_PATH, CKIT_MANAGER,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return QObject::tr(DBUS_FAILED_CONN); }
    QDBusMessage reply = iface.call(CKIT_POWEROFF, true);
    return reply.errorMessage();
}

bool CKit::canSuspend()
{
    QDBusInterface iface(CKIT_SERVICE, CKIT_PATH, CKIT_MANAGER,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    QDBusMessage reply = iface.call(CKIT_CAN_SUSPEND);
    if (reply.arguments().first().toString() == "yes") { return true; }
    return false;
}

QString CKit::suspend()
{
    QDBusInterface iface(CKIT_SERVICE, CKIT_PATH, CKIT_MANAGER,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return QObject::tr(DBUS_FAILED_CONN); }
    QDBusMessage reply = iface.call(CKIT_SUSPEND, true);
    return reply.errorMessage();
}

bool CKit::canHibernate()
{
    QDBusInterface iface(CKIT_SERVICE, CKIT_PATH, CKIT_MANAGER,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    QDBusMessage reply = iface.call(CKIT_CAN_HIBERNATE);
    if (reply.arguments().first().toString() == "yes") { return true; }
    return false;
}

QString CKit::hibernate()
{
    QDBusInterface iface(CKIT_SERVICE, CKIT_PATH, CKIT_MANAGER,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return QObject::tr(DBUS_FAILED_CONN); }
    QDBusMessage reply = iface.call(CKIT_HIBERNATE, true);
    return reply.errorMessage();
}

bool CKit::canHybridSleep()
{
    QDBusInterface iface(CKIT_SERVICE, CKIT_PATH, CKIT_MANAGER,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    QDBusMessage reply = iface.call(CKIT_CAN_HYBRIDSLEEP);
    if (reply.arguments().first().toString() == "yes") { return true; }
    return false;
}

QString CKit::hybridSleep()
{
    QDBusInterface iface(CKIT_SERVICE, CKIT_PATH, CKIT_MANAGER,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return QObject::tr(DBUS_FAILED_CONN); }
    QDBusMessage reply = iface.call(CKIT_HYBRIDSLEEP, true);
    return reply.errorMessage();
}
