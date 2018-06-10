/*
# Power Dwarf <powerdwarf.dracolinux.org>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef COMMON_H
#define COMMON_H

#include <QSettings>
#include <QVariant>

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

#define LID_BATTERY_DEFAULT lidSleep
#define LID_AC_DEFAULT lidLock
#define CRITICAL_DEFAULT criticalNone

#define LOW_BATTERY 15
#define CRITICAL_BATTERY 10
#define AUTO_SLEEP_BATTERY 15
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

class Common
{
public:
    static void savePowerSettings(QString type, QVariant value)
    {
        QSettings settings("powerdwarf", "powerdwarf");
        settings.setValue(type, value);
    }
    static QVariant loadPowerSettings(QString type)
    {
        QSettings settings("powerdwarf", "powerdwarf");
        return settings.value(type);
    }
    static bool validPowerSettings(QString type)
    {
        QSettings settings("powerdwarf", "powerdwarf");
        return settings.value(type).isValid();
    }
};

#endif // COMMON_H
