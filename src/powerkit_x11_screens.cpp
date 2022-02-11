/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018-2022 Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit_x11_screens.h"

QMap<QString, bool> Screens::outputsDpy(Display *dpy)
{
    QMap<QString,bool> result;
    if (dpy == nullptr) { return result; }
    XRRScreenResources *sr;
    XRROutputInfo *info;
    sr = XRRGetScreenResourcesCurrent(dpy, DefaultRootWindow(dpy));
    if (sr) {
        for (int i = 0; i< sr->noutput;++i) {
            info = XRRGetOutputInfo(dpy, sr, sr->outputs[i]);
            if (info == nullptr) {
                XRRFreeOutputInfo(info);
                continue;
            }
            QString output = info->name;
            bool screenConnected = false;
            if (info->connection == RR_Connected) { screenConnected = true; }
            result[output] = screenConnected;
            XRRFreeOutputInfo(info);
        }
    }
    XRRFreeScreenResources(sr);
    return result;
}

QMap<QString, bool> Screens::outputs()
{
    QMap<QString, bool> result;
    Display *dpy;
    if ((dpy = XOpenDisplay(nullptr)) == nullptr) { return result; }
    result = outputsDpy(dpy);
    XCloseDisplay(dpy);
    return result;
}

QString Screens::internalDpy(Display *dpy)
{
    QString result;
    if (dpy == nullptr) { return result; }
    XRRScreenResources *sr;
    sr = XRRGetScreenResourcesCurrent(dpy, DefaultRootWindow(dpy));
    if (sr) {
        XRROutputInfo *info = XRRGetOutputInfo(dpy, sr, sr->outputs[0]);
        if (info) { result = info->name; }
        XRRFreeOutputInfo(info);
    }
    XRRFreeScreenResources(sr);
    return result;
}

QString Screens::internal()
{
    QString result;
    Display *dpy;
    if ((dpy = XOpenDisplay(nullptr)) == nullptr) { return result; }
    result = internalDpy(dpy);
    XCloseDisplay(dpy);
    return result;
}
