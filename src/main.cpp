/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include <QApplication>

#include "powerkit_systray.h"
#include "powerkit_dialog.h"
#include "powerkit_manager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("freedesktop");
    QCoreApplication::setOrganizationDomain("org");
    QCoreApplication::setApplicationVersion(QString::fromUtf8(POWERKIT_VERSION));

    QStringList args = QApplication::arguments();
    if (args.contains("--config")) {
        PowerKit::Dialog dialog;
        dialog.show();
        return a.exec();
    }

    QDBusInterface session(POWERKIT_SERVICE,
                           POWERKIT_PATH,
                           POWERKIT_SERVICE,
                           QDBusConnection::sessionBus());
    if (session.isValid()) {
        qWarning() << QObject::tr("A powerkit session is already running");
        return 1;
    }

    SysTray powerkit(a.parent());
    return a.exec();
}
