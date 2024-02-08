/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include <QApplication>

#include "powerkit_app.h"
#include "powerkit_dialog.h"
#include "powerkit_common.h"
#include "powerkit_backlight.h"

#define CMD_OPT_BRIGHTNESS_UP "--set-brightness-up"
#define CMD_OPT_BRIGHTNESS_DOWN "--set-brightness-down"

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
    } else if (args.contains(CMD_OPT_BRIGHTNESS_UP) ||
               args.contains(CMD_OPT_BRIGHTNESS_DOWN)) {
        int val = PowerKit::Backlight::getCurrentBrightness();
        if (args.contains(CMD_OPT_BRIGHTNESS_UP)) { val += BACKLIGHT_MOVE_VALUE; }
        else if (args.contains(CMD_OPT_BRIGHTNESS_DOWN)) { val -= BACKLIGHT_MOVE_VALUE; }
        return PowerKit::Backlight::setBrightness(val);
    }

    QDBusInterface session(POWERKIT_SERVICE,
                           POWERKIT_PATH,
                           POWERKIT_SERVICE,
                           QDBusConnection::sessionBus());
    if (session.isValid()) {
        qWarning() << QObject::tr("A powerkit session is already running");
        return 1;
    }

    PowerKit::App powerkit(a.parent());
    return a.exec();
}
