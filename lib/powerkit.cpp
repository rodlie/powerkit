/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit.h"

#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusPendingReply>
#include <QXmlStreamReader>
#include <QDebug>

PowerKit::PowerKit(QObject *parent) : QObject(parent)
{
    qDebug() << "start powerkit";
}

PowerKit::~PowerKit()
{
    qDebug() << "end powerkit";
}

bool PowerKit::availableService(const QString &service,
                          const QString &path,
                          const QString &interface)
{
    QDBusInterface iface(service,
                         path,
                         interface,
                         QDBusConnection::systemBus());
    qDebug() << "has service?" << iface.isValid() << service << path << interface;
    if (iface.isValid()) { return true; }
    return false;
}

bool PowerKit::availableAction(const PowerKit::PKMethod &method,
                               const PowerKit::PKBackend &backend)
{
    QString service, path, interface, cmd;
    switch (backend) {
    case PKConsoleKit:
        service = CONSOLEKIT_SERVICE;
        path = CONSOLEKIT_PATH;
        interface = CONSOLEKIT_MANAGER;
        break;
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
    qDebug() << "available action?" << cmd << reply << service << path << interface;
    if (reply.arguments().first().toString() == DBUS_OK_REPLY) { return true; }
    bool result = reply.arguments().first().toBool();
    if (!reply.errorMessage().isEmpty()) { result = false; }
    return result;
}

QString PowerKit::executeAction(const PowerKit::PKAction &action,
                                const PowerKit::PKBackend &backend)
{
    QString service, path, interface, cmd;
    switch (backend) {
    case PKConsoleKit:
        service = CONSOLEKIT_SERVICE;
        path = CONSOLEKIT_PATH;
        interface = CONSOLEKIT_MANAGER;
        break;
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

    qDebug() << "execute action?" << cmd << reply << service << path << interface;
    return reply.errorMessage();
}

QStringList PowerKit::findDevices()
{
    QStringList result;
    QDBusMessage call = QDBusMessage::createMethodCall(UPOWER_SERVICE,
                                                       QString("%1/devices").arg(UPOWER_PATH),
                                                       DBUS_INTROSPECTABLE,
                                                       "Introspect");
    QDBusPendingReply<QString> reply = QDBusConnection::systemBus().call(call);
    QList<QDBusObjectPath> devices;
    QXmlStreamReader xml(reply.value());
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name().toString() == "node" ) {
            QString name = xml.attributes().value("name").toString();
            if(!name.isEmpty()) { devices << QDBusObjectPath(UPOWER_DEVICES + name); }
        }
    }
    foreach (QDBusObjectPath device, devices) {
        result << device.path();
    }
    qDebug() << "found devices" << result;
    return result;
}

bool PowerKit::hasConsoleKit()
{
    return availableService(CONSOLEKIT_SERVICE,
                      CONSOLEKIT_PATH,
                      CONSOLEKIT_MANAGER);
}

bool PowerKit::hasLogind()
{
    return availableService(LOGIND_SERVICE,
                      LOGIND_PATH,
                      LOGIND_MANAGER);
}

bool PowerKit::hasUPower()
{
    return availableService(UPOWER_SERVICE,
                      UPOWER_PATH,
                      UPOWER_MANAGER);
}

bool PowerKit::canRestart()
{
    if (hasLogind()) {
        return availableAction(PKCanRestart, PKLogind);
    } else if (hasConsoleKit()) {
        return availableAction(PKCanRestart, PKConsoleKit);
    }
    return false;
}

bool PowerKit::canPowerOff()
{
    if (hasLogind()) {
        return availableAction(PKCanPowerOff, PKLogind);
    } else if (hasConsoleKit()) {
        return availableAction(PKCanPowerOff, PKConsoleKit);
    }
    return false;
}

bool PowerKit::canSuspend()
{
    if (hasLogind()) {
        return availableAction(PKCanSuspend, PKLogind);
    } else if (hasConsoleKit()) {
        return availableAction(PKCanSuspend, PKConsoleKit);
    } else if (hasUPower()) {
        return availableAction(PKSuspendAllowed, PKUPower);
    }
    return false;
}

bool PowerKit::canHibernate()
{
    if (hasLogind()) {
        return availableAction(PKCanHibernate, PKLogind);
    } else if (hasConsoleKit()) {
        return availableAction(PKCanHibernate, PKConsoleKit);
    } else if (hasUPower()) {
        return availableAction(PKHibernateAllowed, PKUPower);
    }
    return false;
}

bool PowerKit::canHybridSleep()
{
    if (hasLogind()) {
        return availableAction(PKCanHybridSleep, PKLogind);
    } else if (hasConsoleKit()) {
        return availableAction(PKCanHybridSleep, PKConsoleKit);
    }
    return false;
}

QString PowerKit::restart()
{
    if (hasLogind()) {
        return executeAction(PKRestartAction, PKLogind);
    } else if (hasConsoleKit()) {
        return executeAction(PKRestartAction, PKConsoleKit);
    }
    return QObject::tr(PK_NO_BACKEND);
}

QString PowerKit::powerOff()
{
    if (hasLogind()) {
        return executeAction(PKPowerOffAction, PKLogind);
    } else if (hasConsoleKit()) {
        return executeAction(PKPowerOffAction, PKConsoleKit);
    }
    return QObject::tr(PK_NO_BACKEND);
}

QString PowerKit::suspend()
{
    if (hasLogind()) {
        return executeAction(PKSuspendAction, PKLogind);
    } else if (hasConsoleKit()) {
        return executeAction(PKSuspendAction, PKConsoleKit);
    } else if (hasUPower()) {
        return executeAction(PKSuspendAction, PKUPower);
    }
    return QObject::tr(PK_NO_BACKEND);
}

QString PowerKit::hibernate()
{
    if (hasLogind()) {
        return executeAction(PKHibernateAction, PKLogind);
    } else if (hasConsoleKit()) {
        return executeAction(PKHibernateAction, PKConsoleKit);
    } else if (hasUPower()) {
        return executeAction(PKHibernateAction, PKUPower);
    }
    return QObject::tr(PK_NO_BACKEND);
}

QString PowerKit::hybridSleep()
{
    if (hasLogind()) {
        return executeAction(PKHybridSleepAction, PKLogind);
    } else if (hasConsoleKit()) {
        return executeAction(PKHybridSleepAction, PKConsoleKit);
    }
    return QObject::tr(PK_NO_BACKEND);
}
