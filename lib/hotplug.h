/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef HOTPLUG_H
#define HOTPLUG_H

#include <QObject>
#include <QThread>
#include <QMap>

#include "screens.h"
#include <X11/extensions/Xrandr.h>

#undef Bool // fix X11 inc
#define OCNE(X) ((XRROutputChangeNotifyEvent*)X)

class HotPlug : public QObject
{
    Q_OBJECT

public:
    explicit HotPlug(QObject *parent = 0);
    ~HotPlug();

private:
    QThread t;
    bool _scanning;

signals:
    void status(QString display, bool connected);
    void found(QMap<QString,bool> devices);

public slots:
    void requestScan();
    void requestSetScan(bool scanning);
private slots:
    void scan();
    void getScreens(Display *dpy);
    void setScan(bool scanning);
};

#endif // HOTPLUG_H
