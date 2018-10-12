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
#define PM_TIMEOUT 60000
#define PM_MAX_INHIBIT 18000

#define SS_SERVICE "org.freedesktop.ScreenSaver"
#define SS_PATH "/ScreenSaver"
#define SS_TIMEOUT 30000
#define SS_MAX_INHIBIT 18000
#define SS_SIMULATE "SimulateUserActivity"

#define UP_SERVICE "org.freedesktop.UPower"
#define UP_PATH "/org/freedesktop/UPower"
#define UP_CAN_SUSPEND "SuspendAllowed"
#define UP_SUSPEND "Suspend"
#define UP_CAN_HIBERNATE "HibernateAllowed"
#define UP_HIBERNATE "Hibernate"
#define UP_CONN_CHANGED "Changed"
#define UP_CONN_DEV_CHANGED "DeviceChanged"
#define UP_CONN_NOTIFY_RESUME "NotifyResume"
#define UP_CONN_NOTIFY_SLEEP "NotifySleep"
#define UP_DEVICES "/org/freedesktop/UPower/devices/"

#define DBUS_OBJMANAGER "org.freedesktop.DBus.ObjectManager"
#define DBUS_PROPERTIES "org.freedesktop.DBus.Properties"
#define DBUS_INTROSPECTABLE "org.freedesktop.DBus.Introspectable"
#define DBUS_DEVICE "Device"
#define DBUS_DEVICE_ADDED "DeviceAdded"
#define DBUS_DEVICE_REMOVED "DeviceRemoved"
#define DBUS_JOBS "%1/jobs"
#define DBUS_FAILED_CONN "Failed D-Bus connection."

#define CKIT_SERVICE "org.freedesktop.ConsoleKit"
#define CKIT_PATH "/org/freedesktop/ConsoleKit/Manager"
#define CKIT_MANAGER "org.freedesktop.ConsoleKit.Manager"

#define CKIT_CAN_RESTART "CanReboot"
#define CKIT_RESTART "Reboot"
#define CKIT_CAN_POWEROFF "CanPowerOff"
#define CKIT_POWEROFF "PowerOff"
#define CKIT_CAN_SUSPEND "CanSuspend"
#define CKIT_SUSPEND "Suspend"
#define CKIT_CAN_HIBERNATE "CanHibernate"
#define CKIT_HIBERNATE "Hibernate"
#define CKIT_CAN_HYBRIDSLEEP "CanHybridSleep"
#define CKIT_HYBRIDSLEEP "HybridSleep"

#define LOGIN1_SERVICE "org.freedesktop.login1"
#define LOGIN1_PATH "/org/freedesktop/login1"
#define LOGIN1_MANAGER "org.freedesktop.login1.Manager"
#define LOGIN1_CAN_REBOOT "CanReboot"
#define LOGIN1_REBOOT "Reboot"
#define LOGIN1_CAN_POWEROFF "CanPowerOff"
#define LOGIN1_POWEROFF "PowerOff"
#define LOGIN1_CAN_SUSPEND "CanSuspend"
#define LOGIN1_SUSPEND "Suspend"
#define LOGIN1_CAN_HIBERNATE "CanHibernate"
#define LOGIN1_HIBERNATE "Hibernate"
#define LOGIN1_PREP_FOR_SLEEP "PrepareForSleep"

#define XSCREENSAVER "xscreensaver-command -deactivate"
#define XSCREENSAVER_LOCK "xscreensaver-command -lock"

#define MONITOR_DATA_CONNECTED Qt::UserRole+1

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
//#define CONF_BACKLIGHT_LAST "backlight_last"

#define CHECK_UPOWER_TIMEOUT 60000

#define PROP_LOGIN1_DOCKED "Docked"
#define PROP_UPOWER_DOCKED "IsDocked"
#define PROP_UPOWER_LID_IS_PRESENT "LidIsPresent"
#define PROP_UPOWER_LID_IS_CLOSED "LidIsClosed"
#define PROP_UPOWER_ON_BATTERY "OnBattery"
#define PROP_UPOWER_CAN_HIBERNATE "CanHibernate"
#define PROP_UPOWER_CAN_SUSPEND "CanSuspend"
#define PROP_CHANGED "PropertiesChanged"
#define PROP_DEV_MODEL "Model"
#define PROP_DEV_CAPACITY "Capacity"
#define PROP_DEV_IS_RECHARGE "IsRechargeable"
#define PROP_DEV_PRESENT "IsPresent"
#define PROP_DEV_PERCENT "Percentage"
#define PROP_DEV_ENERGY_FULL_DESIGN "EnergyFullDesign"
#define PROP_DEV_ENERGY_FULL "EnergyFull"
#define PROP_DEV_ENERGY_EMPTY "EnergyEmpty"
#define PROP_DEV_ENERGY "Energy"
#define PROP_DEV_ONLINE "Online"
#define PROP_DEV_POWER_SUPPLY "PowerSupply"
#define PROP_DEV_TIME_TO_EMPTY "TimeToEmpty"
#define PROP_DEV_TIME_TO_FULL "TimeToFull"
#define PROP_DEV_TYPE "Type"
#define PROP_DEV_VENDOR "Vendor"
#define PROP_DEV_NATIVEPATH "NativePath"

#endif // DEF_H
