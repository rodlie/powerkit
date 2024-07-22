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

#include "powerkit_common.h"
#include "powerkit_settings.h"

#include <X11/extensions/scrnsaver.h>
#include <X11/extensions/Xrandr.h>

// fix Xrandr
#ifdef Bool
#undef Bool
#endif
#ifdef Unsorted
#undef Unsorted
#endif
//#undef CursorShape
//#undef Status
//#undef None
//#undef KeyPress
//#undef KeyRelease
//#undef FocusIn
//#undef FocusOut
//#undef FontChange
//#undef Expose
//#undef FrameFeature

#define PK_SCREENSAVER_TIMER 10000
#define PK_SCREENSAVER_MAX_INHIBIT 18000
#define PK_SCREENSAVER_ACTIVITY "SimulateUserActivity"

using namespace PowerKit;

ScreenSaver::ScreenSaver(QObject *parent)
    : QObject(parent)
    , blank_time(POWERKIT_SCREENSAVER_TIMEOUT_BLANK)
    , lock_time(POWERKIT_SCREENSAVER_TIMEOUT_LOCK)
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
    if (clients.size() > 0) { return true; }
    return false;
}

void ScreenSaver::timeOut()
{
    if (canInhibit()) {
        SimulateUserActivity();
        return;
    }
    int idle_time = GetSessionIdleTime();
    qDebug() << "screen idle" << idle_time;
    qDebug() << "screen blank timeout" << blank_time;
    qDebug() << "screen lock timeout" << lock_time;
    if (idle_time >= blank_time) {
        setDisplaysOff(true);
    }
    if (idle_time >= lock_time) {
        Lock();
    }
}

void ScreenSaver::Update()
{
    xlock = Settings::getValue(CONF_SCREENSAVER_LOCK_CMD,
                               POWERKIT_SCREENSAVER_LOCK_CMD).toString();
    blank_time = Settings::getValue(CONF_SCREENSAVER_TIMEOUT_BLANK,
                               POWERKIT_SCREENSAVER_TIMEOUT_BLANK).toInt();
    lock_time = Settings::getValue(CONF_SCREENSAVER_TIMEOUT_LOCK,
                               POWERKIT_SCREENSAVER_TIMEOUT_LOCK).toInt();
    int exe1 = QProcess::execute("xset",
                                 QStringList() << "-dpms");
    int exe2 = QProcess::execute("xset",
                                 QStringList() << "s" << "on");
    int exe3 = QProcess::execute("xset",
                                 QStringList() << "s" << QString::number(lock_time));
    qDebug() << "screensaver update" << exe1 << exe2 << exe3;
    if (lock_time < blank_time) {
        int lock_before_blank = QProcess::execute("xset",
                                 QStringList() << "s" << "noblank");
        qDebug() << "screensaver update noblank" << lock_before_blank;
    }
    SimulateUserActivity();
}

void ScreenSaver::Lock()
{
    if (xlock.isEmpty()) { return; }
    qDebug() << "screensaver lock";
    QProcess::startDetached(xlock, QStringList());
}

void ScreenSaver::SimulateUserActivity()
{
    int exe = QProcess::execute("xset",
                                QStringList() << "s" << "reset");
    qDebug() << "screensaver reset" << exe;
}

quint32 ScreenSaver::GetSessionIdleTime()
{
    quint32 idle = 0;
    Display *display = XOpenDisplay(0);
    if (display != 0) {
        XScreenSaverInfo *info = XScreenSaverAllocInfo();
        XScreenSaverQueryInfo(display,
                              DefaultRootWindow(display),
                              info);
        if (info) {
            idle = info->idle;
            XFree(info);
        }
    }
    XCloseDisplay(display);
    quint32 secs = idle / 1000;
    return secs;
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

const QMap<QString, bool> ScreenSaver::GetDisplays()
{
    QMap<QString, bool> result;
    Display *dpy;
    if ((dpy = XOpenDisplay(nullptr)) == nullptr) { return result; }
    XRRScreenResources *sr;
    XRROutputInfo *info;
    sr = XRRGetScreenResourcesCurrent(dpy, DefaultRootWindow(dpy));
    if (sr) {
        for (int i = 0; i < sr->noutput;++i) {
            info = XRRGetOutputInfo(dpy, sr, sr->outputs[i]);
            if (info == nullptr) {
                XRRFreeOutputInfo(info);
                continue;
            }
            QString output = info->name;
            result[output] = (info->connection == RR_Connected);
            XRRFreeOutputInfo(info);
        }
    }
    XRRFreeScreenResources(sr);
    XCloseDisplay(dpy);
    return result;
}

const QString ScreenSaver::GetInternalDisplay()
{
    QString result;
    Display *dpy;
    if ((dpy = XOpenDisplay(nullptr)) == nullptr) { return result; }
    XRRScreenResources *sr;
    sr = XRRGetScreenResourcesCurrent(dpy, DefaultRootWindow(dpy));
    if (sr) {
        XRROutputInfo *info = XRRGetOutputInfo(dpy, sr, sr->outputs[0]);
        if (info) { result = info->name; }
        XRRFreeOutputInfo(info);
    }
    XRRFreeScreenResources(sr);
    XCloseDisplay(dpy);
    return result;
}

void ScreenSaver::setDisplaysOff(bool off)
{
    int xset = QProcess::execute("xset",
                                QStringList() << "dpms" << "force" << (off ? "off" : "on"));
    if (!off) {
        QProcess::execute("xset",
                          QStringList() << "s" << "reset");
    }
    qDebug() << "xset dpms force" << off << xset;
}
