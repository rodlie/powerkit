/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWERKIT_UDEV_H
#define POWERKI_UDEV_H

#include <QObject>
#include <QByteArray>
#include <QSocketNotifier>
#include <QStringList>

#define UDEV_POWER_SUPPLY_CMD "udevadm trigger --verbose --dry-run --type=devices --subsystem-match=power_supply"
#define UDEV_BACKLIGHT_CMD "udevadm trigger --verbose --dry-run --type=devices --subsystem-match=backlight"



class PowerKitUDEV : public QObject
{
    Q_OBJECT

public:
    explicit PowerKitUDEV(QObject *parent = nullptr);
    ~PowerKitUDEV();

private:
    QSocketNotifier *socket_notifier;
    int netlink_socket;
    bool init();
    QStringList getDevices(const QString &cmd);
    QStringList getDeviceInfo(const QString &path);

signals:
    void deviceInfo(const QStringList &info);

private slots:
    void parseDeviceInfo();
};

#endif // POWERKIT_UDEV_H
