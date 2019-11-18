/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include <QCoreApplication>
#include <QLocalSocket>
#include <QDebug>

#include "powerkitd.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QLocalSocket socket;
    socket.connectToServer(POWERKITD_HOST);
    bool connected = socket.waitForConnected(1000);
    if (connected) {
        qDebug("connected to server");
        socket.write("HELLO WORLD!");
        socket.waitForBytesWritten();
        socket.waitForReadyRead();
        qDebug() << socket.readAll();
    }

    return a.exec();
}
