/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit_client.h"
#include <QDebug>

using namespace PowerKit;

double Client::getBatteryLeft(QDBusInterface *iface)
{
    if (!iface) { return -1; }
    qDebug() << "check for battery left";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("BatteryLeft");
    const auto args = reply.arguments();
    double ok = args.last().toDouble();
    qDebug() << "we have battery left" << ok;
    return  ok;
}

bool Client::hasBattery(QDBusInterface *iface)
{
    if (!iface) { return false; }
    qDebug() << "check if we have any battery";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("HasBattery");
    const auto args = reply.arguments();
    bool ok = args.last().toBool();
    qDebug() << "we have any battery?" << ok;
    return  ok;
}

bool Client::onBattery(QDBusInterface *iface)
{
    if (!iface) { return false; }
    qDebug() << "check if we are on battery";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("OnBattery");
    const auto args = reply.arguments();
    bool ok = args.last().toBool();
    qDebug() << "we are on battery?" << ok;
    return  ok;
}

qlonglong Client::timeToEmpty(QDBusInterface *iface)
{
    if (!iface) { return -1; }
    qDebug() << "check for time to empty";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("TimeToEmpty");
    const auto args = reply.arguments();
    qlonglong ok = args.last().toLongLong();
    qDebug() << "we have time to empty?" << ok;
    return  ok;
}

qlonglong Client::timeToFull(QDBusInterface *iface)
{
    if (!iface) { return -1; }
    qDebug() << "check for time to full";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("TimeToFull");
    const auto args = reply.arguments();
    qlonglong ok = args.last().toLongLong();
    qDebug() << "we have time to full?" << ok;
    return  ok;
}

bool Client::canHibernate(QDBusInterface *iface)
{
    if (!iface) { return false; }
    qDebug() << "check if we can hibernate";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("CanHibernate");
    const auto args = reply.arguments();
    bool ok = args.last().toBool();
    qDebug() << "we can hibernate?" << ok;
    return  ok;
}

bool Client::canSuspend(QDBusInterface *iface)
{
    if (!iface) { return false; }
    qDebug() << "check if we can suspend";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("CanSuspend");
    const auto args = reply.arguments();
    bool ok = args.last().toBool();
    qDebug() << "we can suspend?" << ok;
    return  ok;
}

bool Client::canRestart(QDBusInterface *iface)
{
    if (!iface) { return false; }
    qDebug() << "check if we can restart";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("CanRestart");
    const auto args = reply.arguments();
    bool ok = args.last().toBool();
    qDebug() << "we can restart?" << ok;
    return  ok;
}

bool Client::canPowerOff(QDBusInterface *iface)
{
    if (!iface) { return false; }
    qDebug() << "check if we can poweroff";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("CanPowerOff");
    const auto args = reply.arguments();
    bool ok = args.last().toBool();
    qDebug() << "we can poweroff?" << ok;
    return  ok;
}

bool Client::lidIsPresent(QDBusInterface *iface)
{
    if (!iface) { return false; }
    qDebug() << "check if we have a lid";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("LidIsPresent");
    const auto args = reply.arguments();
    bool ok = args.last().toBool();
    qDebug() << "we have a lid?" << ok;
    return  ok;
}

void Client::lockScreen(QDBusInterface *iface)
{
    if (!iface) { return; }
    qDebug() << "lock screen";
    if (!iface->isValid()) { return; }
    QDBusMessage reply = iface->call("LockScreen");
    bool ok = reply.errorMessage().isEmpty();
    qDebug() << "locked screen?" << ok;
}

void Client::hibernate(QDBusInterface *iface)
{
    if (!iface) { return; }
    qDebug() << "hibernate";
    if (!iface->isValid()) { return; }
    QDBusMessage reply = iface->call("Hibernate");
    bool ok = reply.errorMessage().isEmpty();
    qDebug() << "reply" << ok;
}

void Client::suspend(QDBusInterface *iface)
{
    if (!iface) { return; }
    qDebug() << "suspend";
    if (!iface->isValid()) { return; }
    QDBusMessage reply = iface->call("Suspend");
    bool ok = reply.errorMessage().isEmpty();
    qDebug() << "reply" << ok;
}

bool Client::restart(QDBusInterface *iface)
{
    if (!iface) { return false; }
    qDebug() << "restart";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("Restart");
    bool ok = reply.errorMessage().isEmpty();
    qDebug() << "reply" << ok;
    return ok;
}

bool Client::poweroff(QDBusInterface *iface)
{
    if (!iface) { return false; }
    qDebug() << "poweroff";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("PowerOff");
    bool ok = reply.errorMessage().isEmpty();
    qDebug() << "reply" << ok;
    return ok;
}
