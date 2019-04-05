/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef RTC_H
#define RTC_H

#include <QDateTime>

class RTC
{
public:
    static bool setAlarm(const QDateTime &date);
};

#endif // RTC_H
