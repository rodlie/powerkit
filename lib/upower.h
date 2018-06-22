/*
# PowerDwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#
*/

#ifndef UPOWER_H
#define UPOWER_H

#include <QString>

class UPower
{
public:
    static bool canRestart();
    static QString restart();
    static bool canPowerOff();
    static QString poweroff();
    static bool canSuspend();
    static QString suspend();
    static bool canHibernate();
    static QString hibernate();
    static QStringList getDevices();
};

#endif // POWER_H
