/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
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
    suspendShutdown,
    suspendHybrid
};

enum lidAction
{
    lidNone,
    lidLock,
    lidSleep,
    lidHibernate,
    lidShutdown,
    lidHybridSleep
};

enum criticalAction
{
    criticalNone,
    criticalHibernate,
    criticalShutdown
};

#define VIRTUAL_MONITOR "VIRTUAL"
#define TURN_OFF_MONITOR "xrandr --output %1 --off"
#define TURN_ON_MONITOR "xrandr --output %1 --preferred " //--auto"
#define LUMINA_XCONFIG "lumina-xconfig --reset-monitors"

#define LID_BATTERY_DEFAULT lidSleep
#define LID_AC_DEFAULT lidLock
#define CRITICAL_DEFAULT criticalNone

#define LOW_BATTERY 5 // % over critical
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
#define DEFAULT_VIDEO_ICON "video-display"
#define DEFAULT_SUSPEND_ICON "system-suspend"
#define DEFAULT_TRAY_ICON "preferences-other"
#define DEFAULT_NOTIFY_ICON "user-available"
#define DEFAULT_LOCK_ICON "system-lock-screen"
#define DEFAULT_HIBERNATE_ICON "system-hibernate"
#define DEFAULT_SHUTDOWN_ICON "system-shutdown"
#define DEFAULT_NONE_ICON "emblem-unreadable"
#define DEFAULT_BACKLIGHT_ICON "weather-clear"
#define DEFAULT_INFO_ICON "dialog-information"
#define DEFAULT_KEYBOARD_ICON "input-keyboard"
#define DEFAULT_MOUSE_ICON "input-mouse"

#define DEFAULT_SUSPEND_BATTERY_ACTION suspendSleep
#define DEFAULT_SUSPEND_AC_ACTION suspendNone

#define PM_SERVICE "org.freedesktop.PowerManagement"
#define PM_PATH "/PowerManagement"
#define PM_FULL_PATH "/org/freedesktop/PowerManagement"
#define PM_TIMEOUT 60000
#define PM_MAX_INHIBIT 18000

#define SS_SERVICE "org.freedesktop.ScreenSaver"
#define SS_PATH "/ScreenSaver"
#define SS_FULL_PATH "/org/freedesktop/ScreenSaver"
#define SS_TIMEOUT 30000
#define SS_MAX_INHIBIT 18000
#define SS_SIMULATE "SimulateUserActivity"

#define XSCREENSAVER "xscreensaver-command -deactivate"
#define XSCREENSAVER_LOCK "xscreensaver-command -lock"

#define CONF_DIALOG_GEOMETRY "dialog_geometry"
#define CONF_SUSPEND_BATTERY_TIMEOUT "suspend_battery_timeout"
#define CONF_SUSPEND_BATTERY_ACTION "suspend_battery_action"
#define CONF_SUSPEND_AC_TIMEOUT "suspend_ac_timeout"
#define CONF_SUSPEND_AC_ACTION "suspend_ac_action"
#define CONF_CRITICAL_BATTERY_TIMEOUT "critical_battery_timeout"
#define CONF_CRITICAL_BATTERY_ACTION "critical_battery_action"
#define CONF_LID_BATTERY_ACTION "lid_battery_action"
#define CONF_LID_AC_ACTION "lid_ac_action"
#define CONF_LID_DISABLE_IF_EXTERNAL "disable_lid_action_external_monitor"
#define CONF_FREEDESKTOP_SS "freedesktop_ss"
#define CONF_FREEDESKTOP_PM "freedesktop_pm"
#define CONF_TRAY_NOTIFY "tray_notify"
#define CONF_TRAY_SHOW "show_tray"
#define CONF_LID_XRANDR "lid_xrandr_action"
#define CONF_BACKLIGHT_BATTERY "backlight_battery_value"
#define CONF_BACKLIGHT_BATTERY_ENABLE "backlight_battery_enable"
#define CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER "backlight_battery_disable_if_lower"
#define CONF_BACKLIGHT_AC "backlight_ac_value"
#define CONF_BACKLIGHT_AC_ENABLE "backlight_ac_enable"
#define CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER "backlight_ac_disable_if_higher"
#define CONF_DIALOG "dialog_geometry"

#endif // DEF_H
