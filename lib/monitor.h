#ifndef MONITOR_H
#define MONITOR_H

#include <QMap>
#include "common.h"
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/Xinerama.h>

#define INTERNAL_MONITOR "LVDS"
#define VIRTUAL_MONITOR "VIRTUAL"
#define TURN_OFF_MONITOR "xrandr --output %1 --off"
#define TURN_ON_MONITOR "xrandr --output %1"
#define LUMINA_XCONFIG "lumina-xconfig --reset-monitors"

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
    static QString turnOnMonitorUsingXrandr(QString display)
    {
        QString turnOn = QString(TURN_ON_MONITOR).arg(display);
        if (Common::validPowerSettings(QString("%1_mode").arg(display))) {
            turnOn.append(QString(" --mode %1").arg(Common::loadPowerSettings(QString("%1_mode").arg(display)).toString()));
        }
        if (Common::validPowerSettings(QString("%1_rate").arg(display))) {
            turnOn.append(QString(" --rate %1").arg(Common::loadPowerSettings(QString("%1_rate").arg(display)).toString()));
        }
        if (Common::validPowerSettings(QString("%1_rotate").arg(display))) {
            turnOn.append(QString(" --rotate %1").arg(Common::loadPowerSettings(QString("%1_rotate").arg(display)).toString()));
        }
        if (Common::validPowerSettings(QString("%1_option").arg(display)) &&
            Common::validPowerSettings(QString("%1_option_value").arg(display))) {
            switch (Common::loadPowerSettings(QString("%1_option").arg(display)).toInt()) {
            case randrLeftOf:
                turnOn.append(" --left-of");
                break;
            case randrRightOf:
                turnOn.append(" --right-of");
                break;
            case randrAbove:
                turnOn.append(" --above");
                break;
            case randrBelow:
                turnOn.append(" --below");
                break;
            case randrSameAs:
                turnOn.append(" --same-as");
                break;
            default:
                turnOn.append(" --auto");
                break;
            }
            if (!turnOn.contains("--auto")) {
                turnOn.append(QString(" %1").arg(Common::loadPowerSettings(QString("%1_option_value").arg(display)).toString()));
            }
        } else {
            turnOn.append(" --auto");
        }
        if (Common::loadPowerSettings(QString("%1_primary").arg(display)).toBool()) {
            turnOn.append(" --primary");
        }
        return turnOn;
    }
};

#endif // Monitor_H
