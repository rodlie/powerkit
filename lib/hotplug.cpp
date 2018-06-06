/*
# Power Dwarf <powerdwarf.dracolinux.org>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#
*/

#include "hotplug.h"

HotPlug::HotPlug(QObject *parent) :
    QObject(parent)
  , _scanning(false)
{
    moveToThread(&t);
    t.start();
}

HotPlug::~HotPlug()
{
    _scanning = false;
    t.wait();
    t.quit();
}

void HotPlug::requestScan()
{
    QMetaObject::invokeMethod(this, "scan");
}

void HotPlug::scan()
{
    if (_scanning) { return; }
    _scanning = true;

    Display *dpy;
    if ((dpy = XOpenDisplay(NULL)) == NULL) { return; }

    XRRScreenResources *sr;
    XRROutputInfo *info;
    XEvent ev;

    getScreens(dpy);

    XRRSelectInput(dpy, DefaultRootWindow(dpy), RROutputChangeNotifyMask);
    XSync(dpy, 0);
    while(_scanning) {
        if (!XNextEvent(dpy, &ev)) {
            sr = XRRGetScreenResources(OCNE(&ev)->display, OCNE(&ev)->window);
            if (sr == NULL) { continue; }
            info = XRRGetOutputInfo(OCNE(&ev)->display, sr, OCNE(&ev)->output);
            if (info == NULL) {
                XRRFreeScreenResources(sr);
                continue;
            }
            QString screenName = info->name;
            bool screenConnected = false;
            if (info->connection == RR_Connected) { screenConnected = true; }
            emit status(screenName, screenConnected);
            XRRFreeScreenResources(sr);
            XRRFreeOutputInfo(info);
        }
    }
    XCloseDisplay(dpy);
}

void HotPlug::requestSetScan(bool scanning)
{
    QMetaObject::invokeMethod(this, "setScan", Q_ARG(bool, scanning));
}

void HotPlug::getScreens(Display *dpy)
{
    if (dpy == NULL) { return; }
    QMap<QString,bool> result;

    XRRMonitorInfo *mi;
    XRRScreenResources *sr;
    XineramaScreenInfo *si;
    XRROutputInfo *info;
    int i, j, k, monitors, screens = -1;

    si = XineramaQueryScreens(dpy, &screens);
    mi = XRRGetMonitors(dpy, DefaultRootWindow(dpy), True, &monitors);
    sr = XRRGetScreenResourcesCurrent(dpy, DefaultRootWindow(dpy));
    if (si && mi) {
        for (i = 0; i < screens; ++i) {
            for (j = 0; j < monitors; ++j) {
                for (k = 0; k < mi[j].noutput; ++k) {
                    info = XRRGetOutputInfo(dpy, sr, mi[j].outputs[k]);
                    if (info == NULL) {
                        continue;
                        XRRFreeOutputInfo(info);
                    }
                    QString screenName = info->name;
                    bool screenConnected = false;
                    if (info->connection == RR_Connected) { screenConnected = true; }
                    result[screenName] = screenConnected;
                    XRRFreeOutputInfo(info);
                }
            }
        }
        XFree(si);
        XRRFreeMonitors(mi);
    }
    XRRFreeScreenResources(sr);
    emit found(result);
}

void HotPlug::setScan(bool scanning)
{
    _scanning = scanning;
}

