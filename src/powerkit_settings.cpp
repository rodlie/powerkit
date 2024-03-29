/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit_settings.h"

#include <QFile>
#include <QDir>
#include <QSettings>
#include <QDebug>

#include "powerkit_common.h"

#define PK "powerkit"

using namespace PowerKit;

void Settings::setValue(const QString &type,
                        const QVariant &value)
{
    QSettings settings(PK, PK);
    settings.setValue(type, value);
    settings.sync();
}

const QVariant Settings::getValue(const QString &type,
                                  const QVariant &fallback)
{
    QSettings settings(PK, PK);
    return settings.value(type, fallback);
}

bool Settings::isValid(const QString &type)
{
    QSettings settings(PK, PK);
    return settings.value(type).isValid();
}

void Settings::saveDefault()
{
    setValue(CONF_LID_BATTERY_ACTION,
                      POWERKIT_LID_BATTERY_ACTION);
    setValue(CONF_LID_AC_ACTION,
                      POWERKIT_LID_AC_ACTION);
    setValue(CONF_CRITICAL_BATTERY_ACTION,
                      POWERKIT_CRITICAL_ACTION);
    setValue(CONF_CRITICAL_BATTERY_TIMEOUT,
                      POWERKIT_CRITICAL_BATTERY);
    setValue(CONF_SUSPEND_BATTERY_TIMEOUT,
                      POWERKIT_AUTO_SLEEP_BATTERY);
    setValue(CONF_TRAY_NOTIFY,
                      true);
    setValue(CONF_TRAY_SHOW,
                      true);
    setValue(CONF_LID_DISABLE_IF_EXTERNAL,
                      false);
    setValue(CONF_SUSPEND_BATTERY_ACTION,
                      suspendSleep);
    setValue(CONF_SUSPEND_AC_ACTION,
                      suspendNone);
    setValue(CONF_BACKLIGHT_BATTERY_ENABLE,
                      false);
    setValue(CONF_BACKLIGHT_AC_ENABLE,
                      false);
    setValue(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER,
                      false);
    setValue(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER,
                      false);
    setValue(CONF_WARN_ON_LOW_BATTERY,
                      true);
    setValue(CONF_WARN_ON_VERYLOW_BATTERY,
                      true);
    setValue(CONF_NOTIFY_ON_BATTERY,
                      true);
    setValue(CONF_NOTIFY_ON_AC,
                      true);
    setValue(CONF_BACKLIGHT_MOUSE_WHEEL,
                      true);
    setValue(CONF_SUSPEND_LOCK_SCREEN,
                      true);
    setValue(CONF_RESUME_LOCK_SCREEN,
                      false);
}

const QString Settings::getConf()
{
    QString config = QString("%1/.config/powerkit/powerkit.conf")
                     .arg(QDir::homePath());
    if (!QFile::exists(config)) { saveDefault(); }
    return config;
}

const QString Settings::getDir()
{
    QString config = QString("%1/.config/powerkit").arg(QDir::homePath());
    if (!QFile::exists(config)) {
        QDir dir(config);
        dir.mkpath(config);
    }
    return config;
}
