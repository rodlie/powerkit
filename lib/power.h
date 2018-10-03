/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWER_H
#define POWER_H

#include <QObject>
#include <QMap>
#include <QDBusInterface>
#include <QDBusObjectPath>
#include <QTimer>

#include "device.h"

class Power : public QObject
{
    Q_OBJECT

public:
    explicit Power(QObject *parent = NULL);
    QMap<QString,Device*> devices;

private:
    QDBusInterface *upower;
    QDBusInterface *logind;
    QTimer timer;
    bool wasDocked;
    bool wasLidClosed;
    bool wasOnBattery;

signals:
    void updatedDevices();
    void closedLid();
    void openedLid();
    void switchedToBattery();
    void switchedToAC();
    void notifyStatus(QString title, QString msg, bool critical = false);
    void aboutToSuspend();
    void aboutToResume();

public slots:
    bool isDocked();
    bool lidIsPresent();
    bool lidIsClosed();
    bool onBattery();
    bool canHibernate();
    bool canSuspend();
    double batteryLeft();
    void sleep();
    void hibernate();
    void lockScreen();
    void shutdown();
    bool hasBattery();
    qlonglong timeToEmpty();

private slots:
    void setupDBus();
    void scanDevices();
    void deviceAdded(const QDBusObjectPath &obj);
    void deviceRemoved(const QDBusObjectPath &obj);
    void deviceChanged();
    void handleDeviceChanged(QString devicePath);
    void checkUPower();
    void notifyResume();
    void notifySleep();
};

#endif // POWER_H
