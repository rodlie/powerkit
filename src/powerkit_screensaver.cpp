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

using namespace PowerKit;

ScreenSaver::ScreenSaver(QObject *parent) : QObject(parent)
{
    timer.setInterval(PK_SCREENSAVER_TIMER);
    connect(&timer, SIGNAL(timeout()),
            this, SLOT(timeOut()));
    timer.start();
    Update();
}

quint32 ScreenSaver::genCookie()
{
    quint32 cookie = QRandomGenerator::global()->generate();
    while (!clients.contains(cookie)) {
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
            .secsTo(QTime::currentTime()) >= PK_SCREENSAVER_MAX_INHIBIT) {
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
    qDebug() << "screensaver timeout" << canInhibit();
    if (canInhibit()) { SimulateUserActivity(); }
}

void ScreenSaver::pingPM()
{
    QDBusInterface iface(PM_SERVICE, PM_PATH, PM_SERVICE,
                         QDBusConnection::sessionBus());
    if (!iface.isValid()) { return; }
    iface.call(PK_SCREENSAVER_ACTIVITY);
}

void ScreenSaver::Update()
{
    int exe1 = QProcess::execute("xset",
                                 QStringList() << "-dpms");
    int exe2 = QProcess::execute("xset",
                                 QStringList() << "s" << "on");
    int exe3 = QProcess::execute("xset",
                                 QStringList() << "s" << PowerSettings::getValue(PK_SCREENSAVER_CONF_TIMEOUT_BLANK,
                                                                                 PK_SCREENSAVER_TIMEOUT_BLANK).toString());
    qDebug() << "screensaver update" << exe1 << exe2 << exe3;
    SimulateUserActivity();
}

void ScreenSaver::Lock()
{
    qDebug() << "screensaver lock";
}

void ScreenSaver::SimulateUserActivity()
{
    int exe = QProcess::execute("xset",
                                QStringList() << "s" << "reset");
    qDebug() << "screensaver reset" << exe;
}

quint32 ScreenSaver::GetSessionIdleTime()
{
    qDebug() << "screensaver idle";
    return 0;
}

quint32 ScreenSaver::Inhibit(const QString &application_name,
                             const QString &reason_for_inhibit)
{
    quint32 cookie = genCookie();
    emit newInhibit(application_name,
                    reason_for_inhibit,
                    cookie);
    timeOut();
    return cookie;
}

void ScreenSaver::UnInhibit(quint32 cookie)
{
    if (clients.contains(cookie)) { clients.remove(cookie); }
    timeOut();
    emit removedInhibit(cookie);
}
