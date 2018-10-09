/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QDBusInterface>

class Device : public QObject
{
    Q_OBJECT

public:
    enum DeviceType {
        DeviceUnknown,
        DeviceLinePower,
        DeviceBattery,
        DeviceUps,
        DeviceMonitor,
        DeviceMouse,
        DeviceKeyboard,
        DevicePda,
        DevicePhone
    };
    explicit Device(const QString block,
                    QObject *parent = NULL);
    QString name;
    QString path;
    QString model;
    DeviceType type;
    bool isRechargable;
    bool isPresent;
    double percentage;
    bool online;
    bool hasPowerSupply;
    bool isBattery;
    bool isAC;
    QString vendor;
    QString nativePath;
    double capacity;
    double energy;
    double energyFullDesign;
    double energyFull;
    double energyEmpty;
    qlonglong timeToEmpty;
    qlonglong timeToFull;

private:
    QDBusInterface *dbus;
    QDBusInterface *dbusp;

signals:
    void deviceChanged(QString devicePath);

private slots:
    void updateDeviceProperties();
public slots:
    void update();
    void updateBattery();
};

#endif // DEVICE_H
