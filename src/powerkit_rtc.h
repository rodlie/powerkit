/*
# PowerKit <https://github.com/rodlie/powerkit>
# CCopyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWERKIT_RTC_H
#define POWERKIT_RTC_H

#include <QDateTime>

namespace PowerKit
{
    class Rtc
    {
    public:
        static bool setAlarm(const QDateTime &date);
    };
}

#endif // POWERKIT_RTC_H
