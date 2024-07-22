/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWERKIT_COMMON_H
#define POWERKIT_COMMON_H

namespace PowerKit
{
    enum randrAction
    {
        randrAuto,
        randrLeftOf,
        randrRightOf,
        randrAbove,
        randrBelow,
        randrSameAs
    };

    enum suspendAction
    {
        suspendNone,
        suspendSleep,
        suspendHibernate,
        suspendShutdown,
        suspendHybrid,
        suspendSleepHibernate
    };

    enum lidAction
    {
        lidNone,
        lidLock,
        lidSleep,
        lidHibernate,
        lidShutdown,
        lidHybridSleep,
        lidSleepHibernate
    };

    enum criticalAction
    {
        criticalNone,
        criticalHibernate,
        criticalShutdown,
        criticalSuspend
    };
}

#define POWERKIT_LID_BATTERY_ACTION PowerKit::lidSleep
#define POWERKIT_LID_AC_ACTION PowerKit::lidLock
#define POWERKIT_CRITICAL_ACTION PowerKit::criticalNone
#define POWERKIT_SUSPEND_BATTERY_ACTION PowerKit::suspendSleep
#define POWERKIT_SUSPEND_AC_ACTION PowerKit::suspendNone
#define POWERKIT_BACKLIGHT_STEP 10 // +/-
#define POWERKIT_LOW_BATTERY 5 // % over critical
#define POWERKIT_CRITICAL_BATTERY 10 // %
#define POWERKIT_AUTO_SLEEP_BATTERY 15 // min
#define POWERKIT_ICON "ac-adapter"
#define POWERKIT_SERVICE "org.freedesktop.PowerKit"
#define POWERKIT_PATH "/PowerKit"
#define POWERKIT_MANAGER "org.freedesktop.PowerKit.Manager"
#define POWERKIT_CONFIG "org.freedesktop.PowerKit.Configuration"
#define POWERKIT_LOGIND_SERVICE "org.freedesktop.login1"
#define POWERKIT_UPOWER_SERVICE "org.freedesktop.UPower"
#define POWERKIT_DBUS_PROPERTIES "org.freedesktop.DBus.Properties"
#define POWERKIT_SCREENSAVER_LOCK_CMD "xsecurelock"
#define POWERKIT_SCREENSAVER_TIMEOUT_BLANK 300
#define POWERKIT_SCREENSAVER_TIMEOUT_LOCK 600

#endif // POWERKIT_COMMON_H
