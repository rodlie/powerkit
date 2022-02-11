/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018-2022 Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit_freedesktop_ss.h"
#include <QMapIterator>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QCoreApplication>
#include <QProcess>

#include "powerkit_def.h"

ScreenSaver::ScreenSaver(QObject *parent) : QObject(parent)
{
    timer.setInterval(SS_TIMEOUT);
    connect(&timer, SIGNAL(timeout()),
            this, SLOT(timeOut()));
    timer.start();
}

int ScreenSaver::randInt(int low, int high)
{
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
    return qrand() % ((high + 1) - low) + low;
}

quint32 ScreenSaver::genCookie()
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

void ScreenSaver::checkForExpiredClients()
{
    QMapIterator<quint32, QTime> client(clients);
    while (client.hasNext()) {
        client.next();
        if (client.value()
            .secsTo(QTime::currentTime())>=SS_MAX_INHIBIT) {
            clients.remove(client.key());
        }
    }
}

bool ScreenSaver::canInhibit()
{
    checkForExpiredClients();
    if (clients.size()>0) { return true; }
    return false;
}

void ScreenSaver::timeOut()
{
    if (canInhibit()) { SimulateUserActivity(); }
}

void ScreenSaver::pingPM()
{
    QDBusInterface iface(PM_SERVICE, PM_PATH, PM_SERVICE,
                         QDBusConnection::sessionBus());
    if (!iface.isValid()) {
        return;
    }
    iface.call(SS_SIMULATE);
}

void ScreenSaver::SimulateUserActivity()
{
    QProcess proc;
    proc.start(XSCREENSAVER);
    proc.waitForFinished();
    proc.close();
    pingPM();
}

quint32 ScreenSaver::Inhibit(const QString &application,
                             const QString &reason)
{
    quint32 cookie = genCookie();
    emit newInhibit(application, reason, cookie);
    timeOut();
    return cookie;
}

void ScreenSaver::UnInhibit(quint32 cookie)
{
    if (clients.contains(cookie)) { clients.remove(cookie); }
    timeOut();
    emit removedInhibit(cookie);
}
