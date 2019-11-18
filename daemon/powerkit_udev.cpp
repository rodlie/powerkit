/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit_udev.h"

#include <QDebug>
#include <QBuffer>

#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <errno.h>
#include <unistd.h>

/*

  I used https://github.com/wang-bin/qdevicewatcher/blob/master/src/qdevicewatcher_linux.cpp
  as an example for the netlink socket setup.

*/

#define UEVENT_BUFFER_SIZE 2048

enum udev_monitor_netlink_group {
    UDEV_MONITOR_NONE,
    UDEV_MONITOR_KERNEL,
    UDEV_MONITOR_UDEV
};

PowerKitUDEV::PowerKitUDEV(QObject *parent) : QObject(parent)
  , socket_notifier(nullptr)
  , netlink_socket(-1)
{
    qDebug("started udev");
    if (!init()) {
        qWarning("failed to init!");
    }
}

PowerKitUDEV::~PowerKitUDEV()
{
    close(netlink_socket);
    netlink_socket = -1;
    qDebug("ended udev");
}

bool PowerKitUDEV::init()
{
    qDebug("init udev");

    struct sockaddr_nl snl;
    const int buffersize = 16 * 1024 * 1024;
    int retval;

    memset(&snl, 0x00, sizeof(struct sockaddr_nl));
    snl.nl_family = AF_NETLINK;
    snl.nl_groups = UDEV_MONITOR_KERNEL;

    netlink_socket = socket(PF_NETLINK,
                            SOCK_DGRAM,
                            NETLINK_KOBJECT_UEVENT);
    if (netlink_socket == -1) {
        qWarning("error getting socket: %s", strerror(errno));
        return false;
    }

    setsockopt(netlink_socket,
               SOL_SOCKET,
               SO_RCVBUFFORCE,
               &buffersize,
               sizeof(buffersize));
    retval = bind(netlink_socket,
                  (struct sockaddr*) &snl,
                  sizeof(struct sockaddr_nl));
    if (retval < 0) {
        qWarning("bind failed: %s", strerror(errno));
        close(netlink_socket);
        netlink_socket = -1;
        return false;
    } else if (retval == 0) {
        //from libudev-monitor.c
        struct sockaddr_nl _snl;
        socklen_t _addrlen;

        /*
         * get the address the kernel has assigned us
         * it is usually, but not necessarily the pid
         */
        _addrlen = sizeof(struct sockaddr_nl);
        retval = getsockname(netlink_socket, (struct sockaddr *)&_snl, &_addrlen);
        if (retval == 0)
            snl.nl_pid = _snl.nl_pid;
    }

    socket_notifier = new QSocketNotifier(netlink_socket,
                                          QSocketNotifier::Read,
                                          this);
    connect(socket_notifier, SIGNAL(activated(int)),
            this, SLOT(parseDeviceInfo()));
    socket_notifier->setEnabled(true);

    return true;
}

void PowerKitUDEV::parseDeviceInfo()
{
    qDebug("parse device info");
    QByteArray data;

    data.resize(UEVENT_BUFFER_SIZE*2);
    data.fill(0);
    size_t len = read(socket_notifier->socket(), data.data(), UEVENT_BUFFER_SIZE*2);
    data.resize(len);
    data = data.replace(0, '\n').trimmed();

    QStringList result;
    QBuffer buffer;
    buffer.setBuffer(&data);
    buffer.open(QIODevice::ReadOnly);
    while(!buffer.atEnd()) { result.append(buffer.readLine().trimmed()); }
    buffer.close();

    emit deviceInfo(result);
    qDebug() << result;
}
