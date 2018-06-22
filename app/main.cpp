/*
# PowerDwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#
*/

#include <QApplication>
#include "systray.h"
#include "dialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("freedesktop");
    QCoreApplication::setOrganizationDomain("org");
    QCoreApplication::setApplicationVersion("0.9.0");

    // get user input
    QString userArg;
    QString userOpt;
    if (a.arguments().size()>1) { userArg = a.arguments().at(1); }
    if (a.arguments().size()>2) { userOpt = a.arguments().at(2); }

    // trigger system services early
    QDBusInterface systemUP(UP_SERVICE,
                            UP_PATH,
                            UP_SERVICE,
                            QDBusConnection::systemBus());
    QDBusInterface systemCK(CKIT_SERVICE,
                            CKIT_PATH,
                            CKIT_SERVICE,
                            QDBusConnection::systemBus());

    // console actions
    if (userArg == "--suspend") {
        if (UPower::canSuspend()) { UPower::suspend(); }
        else { qWarning() << QObject::tr("Unable to execute requested action"); return 1; }
        return 0;
    } else if (userArg == "--hibernate") {
        if (UPower::canHibernate()) { UPower::hibernate(); }
        else { qWarning() << QObject::tr("Unable to execute requested action"); return 1; }
        return 0;
    } else if (userArg == "--poweroff") {
        if (UPower::canPowerOff()) { UPower::poweroff(); }
        else { qWarning() << QObject::tr("Unable to execute requested action"); return 1; }
        return 0;
    } else if (userArg == "--restart") {
        if (UPower::canRestart()) { UPower::restart(); }
        else { qWarning() << QObject::tr("Unable to execute requested action"); return 1; }
        return 0;
    } else if (userArg == "--config") { // show config dialog
        Dialog dialog;
        dialog.show();
        return a.exec();
    }

    // check if a powerdwarf session is already running
    QDBusInterface session(PD_SERVICE,
                           PD_PATH,
                           PD_SERVICE,
                           QDBusConnection::sessionBus());
    if (session.isValid()) {
        qWarning() << QObject::tr("A powerdwarf session is already running");
        return 1;
    }

    // start systray
    SysTray tray(a.parent());
    return a.exec();
}
