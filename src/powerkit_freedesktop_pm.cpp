/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit_freedesktop_pm.h"
#include <QMapIterator>
#include <QDBusConnection>
#include <QCoreApplication>
#include <QProcess>
#include <QDebug>

#include "powerkit_common.h"

PowerManagement::PowerManagement(QObject *parent) : QObject(parent)
{
    timer.setInterval(PM_TIMEOUT);
    connect(&timer, SIGNAL(timeout()),
            this, SLOT(timeOut()));
    timer.start();
}

int PowerManagement::randInt(int low, int high)
{
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
    return qrand() % ((high + 1) - low) + low;
}

quint32 PowerManagement::genCookie()
{
    int low = 0;
    int high = 1000;
    quint32 cookie = (quint32)randInt(low, high);
    while(!clients.contains(cookie)) {
        if (!clients.contains(cookie)) { clients[cookie] = QTime::currentTime(); }
        else { cookie = (quint32)randInt(low, high); }
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
