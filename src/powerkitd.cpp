/*
# PowerKit <https://github.com/rodlie/powerkit>
# CCopyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include <QCoreApplication>
#include <QtDBus>

#include "powerkitd_manager.h"

#define DSERVICE "org.freedesktop.PowerKit"
#define DPATH "/PowerKit"
#define DFULL_PATH "/org/freedesktop/PowerKit"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationDomain("org");
    QCoreApplication::setApplicationName("freedesktop.PowerKit");

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
