#include "login1.h"
#include <QDBusObjectPath>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusInterface>
#include <QDebug>

#include "def.h"

bool Login1::hasService()
{
    QDBusInterface iface(LOGIN1_SERVICE,
                         LOGIN1_PATH,
                         LOGIN1_MANAGER,
                         QDBusConnection::systemBus());
    if (iface.isValid()) { return true; }
    return false;
}

bool Login1::canRestart()
{
    return false;
}

QString Login1::restart()
{
    return QString();
}

bool Login1::canPowerOff()
{
    return false;
}

QString Login1::poweroff()
{
    return QString();
}

bool Login1::canSuspend()
{
    QDBusInterface iface(LOGIN1_SERVICE,
                         LOGIN1_PATH,
                         LOGIN1_MANAGER,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    QDBusMessage reply = iface.call("CanSuspend");
    qDebug() << "can suspend?" << reply;
    if (reply.arguments().first().toString() == "yes") { return true; }
    return false;
}

QString Login1::suspend()
{
    QDBusInterface iface(LOGIN1_SERVICE,
                         LOGIN1_PATH,
                         LOGIN1_MANAGER,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return QObject::tr("Failed D-Bus connection."); }
    QDBusMessage reply = iface.call("Suspend", true);
    return reply.errorMessage();
}

bool Login1::canHibernate()
{
    QDBusInterface iface(LOGIN1_SERVICE,
                         LOGIN1_PATH,
                         LOGIN1_MANAGER,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    QDBusMessage reply = iface.call("CanHibernate");
    qDebug() << "can hibernate?" << reply;
    if (reply.arguments().first().toString() == "yes") { return true; }
    return false;
}

QString Login1::hibernate()
{
    return QString();
}
