/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit_powermanagement.h"
#include <QMapIterator>
#include <QDBusConnection>
#include <QCoreApplication>
#include <QProcess>
#include <QRandomGenerator>
#include <QDebug>

#include "powerkit_common.h"

using namespace PowerKit;

PowerManagement::PowerManagement(QObject *parent) : QObject(parent)
{
    timer.setInterval(PM_TIMEOUT);
    connect(&timer, SIGNAL(timeout()),
            this, SLOT(timeOut()));
    timer.start();
}

quint32 PowerManagement::genCookie()
{
    quint32 cookie = QRandomGenerator::global()->generate();
    while (!clients.contains(cookie)) {
        if (!clients.contains(cookie)) { clients[cookie] = QTime::currentTime(); }
        else { cookie = QRandomGenerator::global()->generate(); }
    }
    return cookie;
}

void PowerManagement::checkForExpiredClients()
{
    QMapIterator<quint32, QTime> client(clients);
    while (client.hasNext()) {
        client.next();
        if (client.value()
            .secsTo(QTime::currentTime())>=PM_MAX_INHIBIT) {
            clients.remove(client.key());
        }
    }
}

bool PowerManagement::canInhibit()
{
    checkForExpiredClients();
    if (clients.size()>0) { return true; }
    return false;
}

void PowerManagement::timeOut()
{
    if (canInhibit()) { SimulateUserActivity(); }
}

void PowerManagement::SimulateUserActivity()
{
    qDebug() << "SimulateUserActivity";
    emit HasInhibitChanged(true);
}

quint32 PowerManagement::Inhibit(const QString &application,
                                 const QString &reason)
{
    qDebug() << "Inhibit" << application << reason;
    quint32 cookie = genCookie();
    timeOut();
    emit newInhibit(application, reason, cookie);
    emit HasInhibitChanged(canInhibit());
    return cookie;
}

void PowerManagement::UnInhibit(quint32 cookie)
{
    qDebug() << "UnInhibit" << cookie;
    if (clients.contains(cookie)) { clients.remove(cookie); }
    timeOut();
    emit removedInhibit(cookie);
    emit HasInhibitChanged(canInhibit());
}

bool PowerManagement::HasInhibit()
{
    return canInhibit();
}
