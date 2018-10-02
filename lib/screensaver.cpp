/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "screensaver.h"
#include <QMapIterator>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QCoreApplication>
#include <QProcess>

#include "def.h"

ScreenSaver::ScreenSaver()
{
    timer.setInterval(SS_TIMEOUT);
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeOut()));
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
    quint32 cookie = randInt(low, high);
    while(!clients.contains(cookie)) {
        if (!clients.contains(cookie)) { clients[cookie] = QTime::currentTime(); }
        else { cookie = randInt(low, high); }
    }
    return cookie;
}

void ScreenSaver::checkForExpiredClients()
{
    QMapIterator<quint32, QTime> client(clients);
    while (client.hasNext()) {
        client.next();
        if (client.value().secsTo(QTime::currentTime())>=SS_MAX_INHIBIT) { clients.remove(client.key()); }
    }
}

bool ScreenSaver::canInhibit()
{
    checkForExpiredClients();
    if (clients.size()>0) { return true; }
    return false;
}

void ScreenSaver::checkForDBusSession()
{
    //if (!QDBusConnection::sessionBus().isConnected()) {
    // DBus session has probably ended(?), so quit ...
    //qApp->quit();
    //}
}

void ScreenSaver::timeOut()
{
    checkForDBusSession();
    if (canInhibit()) { SimulateUserActivity(); }
}

void ScreenSaver::pingPM()
{
    QDBusInterface iface(PM_SERVICE, PM_PATH, PM_SERVICE,
                         QDBusConnection::sessionBus());
    if (!iface.isValid()) {
        return;
    }
    iface.call("SimulateUserActivity");
}

void ScreenSaver::SimulateUserActivity()
{
    QProcess proc;
    proc.start(XSCREENSAVER);
    proc.waitForFinished();
    proc.close();
    pingPM();
}

quint32 ScreenSaver::Inhibit(QString application, QString reason)
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
