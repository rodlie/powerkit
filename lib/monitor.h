/*
# PowerDwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#
*/

#ifndef MONITOR_H
#define MONITOR_H

#include <QMap>
#include <QStringList>
#include <QProcess>
#include <QVector>

#include "common.h"
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/Xinerama.h>

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
    static QMap<QString,bool> get(Display *dpy);
    static QMap<QString,bool> getX();
    static QString turnOffMonitorUsingXrandr(QString display);
    static QString turnOnMonitorUsingXrandr(QString display);
    static monitorInfo getMonitorInfo(QString display);
};

#endif // Monitor_H
