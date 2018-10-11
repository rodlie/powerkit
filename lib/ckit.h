/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef CKIT_H
#define CKIT_H

#include <QString>

class CKit
{
public:
    static bool hasService();
    static bool canRestart();
    static QString restart();
    static bool canPowerOff();
    static QString poweroff();
    static bool canSuspend();
    static QString suspend();
    static bool canHibernate();
    static QString hibernate();
    static bool canHybridSleep();
    static QString hybridSleep();
};

#endif // CKIT_H
