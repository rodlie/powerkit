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

signals:
    void deviceInfo(const QStringList &info);

private slots:
    void parseDeviceInfo();
};

#endif // POWERKIT_UDEV_H
