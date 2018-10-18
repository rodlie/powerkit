/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include <QApplication>
#include "systray.h"
#include "dialog.h"

#include "powerkit.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("freedesktop");
    QCoreApplication::setOrganizationDomain("org");
    QString version = APP_VERSION;
    version.append(APP_VERSION_EXTRA);
    QCoreApplication::setApplicationVersion(version);

    // get user input
    QString userArg;
    QString userOpt;
    if (a.arguments().size()>1) { userArg = a.arguments().at(1); }
    if (a.arguments().size()>2) { userOpt = a.arguments().at(2); }

    // console actions
    if (userArg == "--config") { // show config dialog
        Dialog dialog;
        dialog.show();
        return a.exec();
    }

    // check if a powerkit session is already running
    QDBusInterface session(POWERKIT_SERVICE,
                           POWERKIT_PATH,
                           POWERKIT_SERVICE,
                           QDBusConnection::sessionBus());
    if (session.isValid()) {
        qWarning() << QObject::tr("A powerkit session is already running");
        return 1;
    }

    // start systray
    SysTray tray(a.parent());
    return a.exec();
}
