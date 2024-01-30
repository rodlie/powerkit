/*
# PowerKit <https://github.com/rodlie/powerkit>
# CCopyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit_rtc.h"

#ifdef Q_OS_LINUX
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#define RTC_DEV "/dev/rtc"
#endif

using namespace PowerKit;

bool Rtc::setAlarm(const QDateTime &date)
{
#ifdef Q_OS_LINUX
    if (!date.isValid() || date.isNull()) { return false; }

    int fd, result = 0;
    struct rtc_time rtc;

    fd = open(RTC_DEV, O_RDONLY);
    if (fd ==  -1) { return false; }

    result = ioctl(fd, RTC_UIE_OFF, 0);
    if (result == -1) {
        close(fd);
        return false;
    }

    result = ioctl(fd, RTC_RD_TIME, &rtc);
    if (result == -1) {
        close(fd);
        return false;
    }

    rtc.tm_year = date.date().year();
    rtc.tm_mon = date.date().month();
    rtc.tm_mday = date.date().day();
    rtc.tm_hour = date.time().hour();
    rtc.tm_min = date.time().minute();
    rtc.tm_sec = date.time().second();

    result = ioctl(fd, RTC_ALM_SET, &rtc);
    if (result == -1) {
        close(fd);
        return false;
    }

    result = ioctl(fd, RTC_AIE_ON, 0);
    if (result == -1) {
        close(fd);
        return false;
    }

    close(fd);
    return true;
#else
    Q_UNUSED(date)
    return false;
#endif
}
