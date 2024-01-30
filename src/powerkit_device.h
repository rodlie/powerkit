/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWERKIT_DEVICE_H
#define POWERKIT_DEVICE_H

#include <QObject>
#include <QDBusInterface>

namespace PowerKit
{
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
                        QObject *parent = nullptr);
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
        void deviceChanged(const QString &devicePath);

    private slots:
        void updateDeviceProperties();

    public slots:
        void update();
        void updateBattery();
    };
}

#endif // POWERKIT_DEVICE_H
