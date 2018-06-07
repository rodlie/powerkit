#ifndef MONITOR_H
#define MONITOR_H

#include <QMap>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/Xinerama.h>

class Monitor
{
public:
    static QMap<QString,bool> get(Display *dpy)
    {
        QMap<QString,bool> result;
        if (dpy == NULL) { return result; }

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
        return result;
    }
    static QMap<QString,bool> getX()
    {
        QMap<QString, bool> result;
        Display *dpy;
        if ((dpy = XOpenDisplay(NULL)) == NULL) { return result; }
        result = get(dpy);
        XCloseDisplay(dpy);
        return result;
    }
};

#endif // Monitor_H
