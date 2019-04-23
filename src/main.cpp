/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include <QApplication>
#include "powerkit_systray.h"
#include "powerkit_dialog.h"
#include "powerkit.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("freedesktop");
    QCoreApplication::setOrganizationDomain("org");
    QString version = POWERKIT_VERSION;
#ifdef POWERKIT_VERSION_EXTRA
     if (!QString(POWERKIT_VERSION_EXTRA).isEmpty()) { version.append(POWERKIT_VERSION_EXTRA); }
#endif
    QCoreApplication::setApplicationVersion(version);

    // check args
    bool startConfig = false;
    for (int i=1; i<argc; i++) {if (QString::fromLocal8Bit(argv[i]) == "--config") {
            startConfig = true;
        }
    }
     if (startConfig) { // show config dialog
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

    // start powerkit systray
    SysTray tray(a.parent());
    return a.exec();
}
