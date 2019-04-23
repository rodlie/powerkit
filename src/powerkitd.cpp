/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include <QCoreApplication>
#include <QtDBus>

#include "powerkitd_manager.h"

#define DSERVICE "org.freedesktop.powerkitd"
#define DPATH "/powerkitd"
#define DFULL_PATH "/org/freedesktop/powerkitd"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationDomain("org");
    QCoreApplication::setApplicationName("freedesktop.powerkitd");

    if (!QDBusConnection::systemBus().isConnected()) {
        qWarning("Cannot connect to the D-Bus system bus.");
        return 1;
    }

    if (!QDBusConnection::systemBus().registerService(DSERVICE)) {
        qWarning() << QDBusConnection::systemBus().lastError().message();
        return 1;
    }

    Manager man;
    if (!QDBusConnection::systemBus().registerObject(DPATH,
                                                     &man,
                                                     QDBusConnection::ExportAllContents)) {
        qWarning() << QDBusConnection::systemBus().lastError().message();
        return 1;
    }
    if (!QDBusConnection::systemBus().registerObject(DFULL_PATH,
                                                     &man,
                                                     QDBusConnection::ExportAllContents)) {
        qWarning() << QDBusConnection::systemBus().lastError().message();
        return 1;
    }

    return a.exec();
}
