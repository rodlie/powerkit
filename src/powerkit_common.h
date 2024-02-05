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
        criticalShutdown,
        criticalSuspend
    };
}

#define LID_BATTERY_DEFAULT PowerKit::lidSleep
#define LID_AC_DEFAULT PowerKit::lidLock
#define CRITICAL_DEFAULT PowerKit::criticalNone

#define BACKLIGHT_MOVE_VALUE 10
#define LOW_BATTERY 5 // % over critical
#define CRITICAL_BATTERY 10
#define AUTO_SLEEP_BATTERY 15
#define DEFAULT_THEME "Adwaita"
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
#define DEFAULT_ABOUT_ICON "dialog-question"
#define DEFAULT_SETTINGS_ICON "preferences-system"
#define DEFAULT_HELP_ICON "system-help"
#define DEFAULT_INHIBITOR_ICON "preferences-desktop-screensaver"
#define DEFAULT_APP_ICON "applications-system"

#define DEFAULT_SUSPEND_BATTERY_ACTION PowerKit::suspendSleep
#define DEFAULT_SUSPEND_AC_ACTION PowerKit::suspendNone

#define POWERKIT_SERVICE "org.freedesktop.PowerKit"
#define POWERKIT_PATH "/PowerKit"
#define POWERKIT_FULL_PATH "/org/freedesktop/PowerKit"
#define POWERKIT_MANAGER "org.freedesktop.PowerKit.Manager"

#define PMD_SERVICE "org.freedesktop.PowerKit"
#define PMD_PATH "/org/freedesktop/PowerKit"
#define PMD_MANAGER "org.freedesktop.PowerKit.Manager"

#define PM_SERVICE "org.freedesktop.PowerManagement"
#define PM_PATH "/PowerManagement"
#define PM_FULL_PATH "/org/freedesktop/PowerManagement"
#define PM_TIMEOUT 60000
#define PM_MAX_INHIBIT 18000

#define PM_SERVICE_INHIBIT "org.freedesktop.PowerManagement.Inhibit"
#define PM_FULL_PATH_INHIBIT "/org/freedesktop/PowerManagement/Inhibit"

#define UPOWER_SERVICE "org.freedesktop.UPower"
#define DBUS_PROPERTIES "org.freedesktop.DBus.Properties"
#define DBUS_DEVICE "Device"
#define DBUS_CHANGED "Changed"

// config keys
#define CONF_DIALOG_GEOMETRY "dialog_geometry"
#define CONF_SUSPEND_BATTERY_TIMEOUT "suspend_battery_timeout"
#define CONF_SUSPEND_BATTERY_ACTION "suspend_battery_action"
#define CONF_SUSPEND_AC_TIMEOUT "suspend_ac_timeout"
#define CONF_SUSPEND_AC_ACTION "suspend_ac_action"
#define CONF_SUSPEND_WAKEUP_HIBERNATE_BATTERY "suspend_wakeup_hibernate_battery"
#define CONF_SUSPEND_WAKEUP_HIBERNATE_AC "suspend_wakeup_hibernate_ac"
#define CONF_CRITICAL_BATTERY_TIMEOUT "critical_battery_timeout"
#define CONF_CRITICAL_BATTERY_ACTION "critical_battery_action"
#define CONF_LID_BATTERY_ACTION "lid_battery_action"
#define CONF_LID_AC_ACTION "lid_ac_action"
#define CONF_LID_DISABLE_IF_EXTERNAL "disable_lid_action_external_monitor"
#define CONF_TRAY_NOTIFY "tray_notify"
#define CONF_TRAY_SHOW "show_tray"
#define CONF_LID_XRANDR "lid_xrandr_action"
#define CONF_BACKLIGHT_BATTERY "backlight_battery_value"
#define CONF_BACKLIGHT_BATTERY_ENABLE "backlight_battery_enable"
#define CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER "backlight_battery_disable_if_lower"
#define CONF_BACKLIGHT_AC "backlight_ac_value"
#define CONF_BACKLIGHT_AC_ENABLE "backlight_ac_enable"
#define CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER "backlight_ac_disable_if_higher"
#define CONF_BACKLIGHT_MOUSE_WHEEL "backlight_mouse_wheel"
#define CONF_DIALOG "dialog_geometry"
#define CONF_WARN_ON_LOW_BATTERY "warn_on_low_battery"
#define CONF_WARN_ON_VERYLOW_BATTERY "warn_on_verylow_battery"
#define CONF_NOTIFY_ON_BATTERY "notify_on_battery"
#define CONF_NOTIFY_ON_AC "notify_on_ac"
#define CONF_NOTIFY_NEW_INHIBITOR "notify_new_inhibitor"
#define CONF_SUSPEND_LOCK_SCREEN "lock_screen_on_suspend"
#define CONF_RESUME_LOCK_SCREEN "lock_screen_on_resume"
#define CONF_ICON_THEME "icon_theme"
#define CONF_NATIVE_THEME "native_theme"
#define CONF_KERNEL_BYPASS "kernel_cmd_bypass"
#define CONF_SCREENSAVER_LOCK_CMD "screensaver_lock_cmd"
#define CONF_SCREENSAVER_TIMEOUT_BLANK "screensaver_blank_timeout"

// screensaver

#define PK_SCREENSAVER_LOCK_CMD "xsecurelock"
#define PK_SCREENSAVER_SERVICE "org.freedesktop.ScreenSaver"
#define PK_SCREENSAVER_PATH_ROOT "/ScreenSaver"
#define PK_SCREENSAVER_PATH_FULL "/org/freedesktop/ScreenSaver"

#endif // POWERKIT_COMMON_H
