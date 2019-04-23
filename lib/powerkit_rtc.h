/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWER_RTC_H
#define POWER_RTC_H

#include <QDateTime>

class PowerRtc
{
public:
    static bool setAlarm(const QDateTime &date);
};

#endif // POWER_RTC_H
