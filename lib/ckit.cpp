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
    bool result = reply.arguments().first().toBool();
    if (!reply.errorMessage().isEmpty()) { result = false; }
    return result;
}

QString CKit::restart()
{
    QDBusInterface iface(CKIT_SERVICE, CKIT_PATH, CKIT_MANAGER,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return QObject::tr("Failed D-Bus connection."); }
    QDBusMessage reply = iface.call(CKIT_RESTART);
    return reply.errorMessage();
}

bool CKit::canPowerOff()
{
    QDBusInterface iface(CKIT_SERVICE, CKIT_PATH, CKIT_MANAGER,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    QDBusMessage reply = iface.call(CKIT_CAN_POWEROFF);
    bool result = reply.arguments().first().toBool();
    if (!reply.errorMessage().isEmpty()) { result = false; }
    return result;
}

QString CKit::poweroff()
{
    QDBusInterface iface(CKIT_SERVICE, CKIT_PATH, CKIT_MANAGER,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return QObject::tr("Failed D-Bus connection."); }
    QDBusMessage reply = iface.call(CKIT_POWEROFF);
    return reply.errorMessage();
}
