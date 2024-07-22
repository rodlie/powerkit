/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWERKIT_SETTINGS_H
#define POWERKIT_SETTINGS_H

#include <QVariant>
#include <QString>

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
#define CONF_SCREENSAVER_TIMEOUT_LOCK "screensaver_lock_timeout"

namespace PowerKit
{
    class Settings
    {
    public:
        static void setValue(const QString &type,
                             const QVariant &value);
        static const QVariant getValue(const QString &type,
                                       const QVariant &fallback = QVariant());
        static bool isValid(const QString &type);
        static void saveDefault();
        static const QString getConf();
        static const QString getDir();
    };
}

#endif // POWERKIT_SETTINGS_H
