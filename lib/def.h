/*
# PowerDwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#
*/

#ifndef DEF_H
#define DEF_H

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
    suspendShutdown
};

enum lidAction
{
    lidNone,
    lidLock,
    lidSleep,
    lidHibernate,
    lidShutdown
};

enum criticalAction
{
    criticalNone,
    criticalHibernate,
    criticalShutdown
};

#define XRANDR "xrandr"
#define INTERNAL_MONITOR "LVDS"
#define VIRTUAL_MONITOR "VIRTUAL"
#define TURN_OFF_MONITOR "xrandr --output %1 --off"
#define TURN_ON_MONITOR "xrandr --output %1"
#define LUMINA_XCONFIG "lumina-xconfig --reset-monitors"

#define LID_BATTERY_DEFAULT lidSleep
#define LID_AC_DEFAULT lidLock
#define CRITICAL_DEFAULT criticalNone

#define LOW_BATTERY 15
#define CRITICAL_BATTERY 10
#define AUTO_SLEEP_BATTERY 15
#define DEFAULT_AC_ICON "ac-adapter"
#define DEFAULT_BATTERY_ICON "battery"
#define DEFAULT_BATTERY_ICON_CRIT "battery-caution"
#define DEFAULT_BATTERY_ICON_CRIT_AC "battery-caution-charging"
#define DEFAULT_BATTERY_ICON_LOW "battery-low"
#define DEFAULT_BATTERY_ICON_LOW_AC "battery-low-charging"
#define DEFAULT_BATTERY_ICON_GOOD "battery-good"
#define DEFAULT_BATTERY_ICON_GOOD_AC "battery-good-charging"
#define DEFAULT_BATTERY_ICON_FULL "battery-full"
#define DEFAULT_BATTERY_ICON_FULL_AC "battery-full-charging"
#define DEFAULT_BATTERY_ICON_CHARGED "battery-full-charged"
#define DEFAULT_BATTERY_EMPTY "battery-empty"
#define DEFAULT_BATTERY_MISSING "battery-missing"

#define DEFAULT_SUSPEND_BATTERY_ACTION suspendSleep
#define DEFAULT_SUSPEND_AC_ACTION suspendNone

#define PM_SERVICE "org.freedesktop.PowerManagement"
#define PM_PATH "/PowerManagement"
#define PM_TIMEOUT 60000
#define PM_MAX_INHIBIT 18000

#define SS_SERVICE "org.freedesktop.ScreenSaver"
#define SS_PATH "/ScreenSaver"
#define SS_TIMEOUT 30000
#define SS_MAX_INHIBIT 18000

#define UP_SERVICE "org.freedesktop.UPower"
#define UP_PATH "/org/freedesktop/UPower"

#define PD "powerdwarf"
#define PD_SERVICE "org.freedesktop.PowerDwarf"
#define PD_PATH "/PowerDwarf"

#define DBUS_OBJMANAGER "org.freedesktop.DBus.ObjectManager"
#define DBUS_PROPERTIES "org.freedesktop.DBus.Properties"
#define DBUS_INTROSPECTABLE "org.freedesktop.DBus.Introspectable"
#define DBUS_DEVICE_ADDED "DeviceAdded"
#define DBUS_DEVICE_REMOVED "DeviceRemoved"

#define CKIT_SERVICE "org.freedesktop.ConsoleKit"
#define CKIT_PATH "/org/freedesktop/ConsoleKit/Manager"
#define CKIT_MANAGER "org.freedesktop.ConsoleKit.Manager"

#define XSCREENSAVER "xscreensaver-command -deactivate"
#define XSCREENSAVER_LOCK "xscreensaver-command -lock"

#define MONITOR_DATA_CONNECTED Qt::UserRole+1

#endif // DEF_H
