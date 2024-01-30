/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit_screensaver.h"
#include <QMapIterator>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QCoreApplication>
#include <QProcess>
#include <QRandomGenerator>
#include <QDebug>

#include "powerkit_def.h"
#include "powerkit_settings.h"

ScreenSaver::ScreenSaver(QObject *parent) : QObject(parent)
{
    timer.setInterval(SS_TIMEOUT);
    connect(&timer, SIGNAL(timeout()),
            this, SLOT(timeOut()));
    timer.start();
    Update();
}

quint32 ScreenSaver::genCookie()
{
    quint32 cookie = QRandomGenerator::global()->generate();
    while(!clients.contains(cookie)) {
        if (!clients.contains(cookie)) { clients[cookie] = QTime::currentTime(); }
        else { cookie = QRandomGenerator::global()->generate(); }
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
    if (!iface.isValid()) { return; }
    iface.call(SS_SIMULATE);
}

void ScreenSaver::Update()
{
    int exe1 = QProcess::execute("xset",
                                 QStringList() << "-dpms");
    int exe2 = QProcess::execute("xset",
                                 QStringList() << "s" << "on");
    int exe3 = QProcess::execute("xset",
                                 QStringList() << "s" << PowerSettings::getValue(CONF_SCREENSAVER_BLANK_TIMEOUT,
                                                                                 SS_BLANK_TIMEOUT).toString());
    qDebug() << "screensaver update" << exe1 << exe2 << exe3;
    SimulateUserActivity();
}

void ScreenSaver::SimulateUserActivity()
{
    int exe = QProcess::execute("xset",
                                QStringList() << "s" << "reset");
    qDebug() << "screensaver reset" << exe;
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
