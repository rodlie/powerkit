#ifndef MONITOR_H
#define MONITOR_H

#include <QMap>
#include <QStringList>
#include <QProcess>
#include <QVector>

#include "common.h"
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/Xinerama.h>

#define XRANDR "xrandr"
#define INTERNAL_MONITOR "LVDS"
#define VIRTUAL_MONITOR "VIRTUAL"
#define TURN_OFF_MONITOR "xrandr --output %1 --off"
#define TURN_ON_MONITOR "xrandr --output %1"
#define LUMINA_XCONFIG "lumina-xconfig --reset-monitors"

struct monitorInfo
{
    QString display;
    QString currentMode;
    QString currentRate;
    QString preferredRate;
    QVector<QStringList> modes;
    bool isPrimary;
    QString rotate;
    int position;
    QString positionOther;
}; Q_DECLARE_METATYPE(monitorInfo)

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
            QString otherScreen = Common::loadPowerSettings(QString("%1_option_value").arg(display)).toString();
            if (!turnOn.contains("--auto") && otherScreen != QObject::tr("None")) {
                turnOn.append(QString(" %1").arg(otherScreen));
            }
        } else {
            turnOn.append(" --auto");
        }
        if (Common::loadPowerSettings(QString("%1_primary").arg(display)).toBool()) {
            turnOn.append(" --primary");
        }
        return turnOn;
    }
    static monitorInfo getMonitorInfo(QString display)
    {
        monitorInfo result;
        result.isPrimary = false;
        result.position = randrAuto;
        if (display.isEmpty()) { return result; }
        QProcess proc;
        proc.start(XRANDR);
        proc.waitForFinished();
        QString xrandr = proc.readAll();
        QStringList xrandrList = xrandr.split("\n");
        bool foundDisplay = false;
        for (int i=0;i<xrandrList.size();++i) {
            QString line =  xrandrList.at(i);
            if (line.startsWith(display)) { foundDisplay = true; }
            if (foundDisplay &&
                line.contains("connected") &&
                !line.contains(display)) { foundDisplay = false; }
            if (foundDisplay) {
                if (line.contains(display)) {
                    if (line.contains("primary")) { result.isPrimary = true; }
                    QStringList info = line.split(" ", QString::SkipEmptyParts);
                    int foundRotation = 0;
                    for (int z=0;z<info.size();++z) {
                        QString spec = info.at(z);
                        if (spec.count("x")==1 && spec.count("+")==2) {
                            result.currentMode = spec.split("+").takeFirst();
                            foundRotation = z+1;
                        }
                    }
                    if (foundRotation>0) {
                        QString rotation = info.at(foundRotation);
                        if (!rotation.isEmpty() && !rotation.startsWith("(")) {
                            result.rotate = rotation;
                        }
                    }
                    continue;
                }
                QStringList screenMode = line.split(" ", QString::SkipEmptyParts);
                for (int y=0;y<screenMode.size();++y) {
                    QString rate = screenMode.at(y);
                    if (rate.contains("*")) { result.currentRate = QString(rate).replace("*","").replace("+",""); }
                    if (rate.contains("+")) { result.preferredRate = QString(rate).replace("*","").replace("+",""); }
                }
                result.modes.append(screenMode);
            }
        }
        qDebug() << "is primary?" << result.isPrimary;
        qDebug() << "current mode" << result.currentMode;
        qDebug() << "current rate" << result.currentRate;
        qDebug() << "preferred rate" << result.preferredRate;
        qDebug() << "available modes" << result.modes;
        qDebug() << "rotation" << result.rotate;
        return result;
    }
};

#endif // Monitor_H
