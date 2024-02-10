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
#include "powerkit_client.h"

#define CMD_OPT_CONFIG "--config"
#define CMD_OPT_BRIGHTNESS_UP "--set-brightness-up"
#define CMD_OPT_BRIGHTNESS_DOWN "--set-brightness-down"
#define CMD_OPT_SLEEP "--sleep"
#define CMD_OPT_HIBERNATE "--hibernate"
#define CMD_OPT_LOCK "--lock"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("freedesktop");
    QCoreApplication::setOrganizationDomain("org");
    QCoreApplication::setApplicationVersion(QString::fromUtf8(POWERKIT_VERSION));

    const auto args = QApplication::arguments();
    bool openConfig = args.contains(CMD_OPT_CONFIG);
    bool setBrightness = (args.contains(CMD_OPT_BRIGHTNESS_UP) ||
                          args.contains(CMD_OPT_BRIGHTNESS_DOWN));
    bool setSleep = args.contains(CMD_OPT_SLEEP);
    bool setHibernate = args.contains(CMD_OPT_HIBERNATE);
    bool setLock = args.contains(CMD_OPT_LOCK);

    if (openConfig) {
        if (!QDBusConnection::sessionBus().registerService(POWERKIT_CONFIG)) {
            qWarning() << QObject::tr("A powerkit config instance is already running");
            return 1;
        }
        PowerKit::Dialog dialog;
        dialog.show();
        return a.exec();
    } else if (setBrightness) {
        int val = PowerKit::Backlight::getCurrentBrightness();
        if (args.contains(CMD_OPT_BRIGHTNESS_UP)) { val += POWERKIT_BACKLIGHT_STEP; }
        else if (args.contains(CMD_OPT_BRIGHTNESS_DOWN)) { val -= POWERKIT_BACKLIGHT_STEP; }
        return PowerKit::Backlight::setBrightness(val);
    } else if (setSleep || setHibernate || setLock) {
        QDBusInterface manager(POWERKIT_SERVICE,
                               POWERKIT_PATH,
                               POWERKIT_MANAGER,
                               QDBusConnection::sessionBus());
        if (!manager.isValid()) {
            qWarning() << QObject::tr("Unable to connect to the powerkit service");
            return 1;
        }
        if (setSleep && setHibernate) {
            return PowerKit::Client::suspendThenHibernate(&manager);
        } else if (setSleep) {
            return PowerKit::Client::suspend(&manager);
        } else if (setHibernate) {
            return PowerKit::Client::hibernate(&manager);
        } else if (setLock) {
            return PowerKit::Client::lockScreen(&manager);
        }
        return 1;
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
