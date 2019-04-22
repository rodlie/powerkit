/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "manager.h"
#include "rtc.h"
#include "common.h"

#include <QDebug>

Manager::Manager(QObject *parent) : QObject(parent)
{
}

bool Manager::setWakeAlarm(const QString &alarm)
{
    qDebug() << "Try to set RTC wake alarm" << alarm;
    QDateTime date = QDateTime::fromString(alarm, "yyyy-MM-dd HH:mm:ss");
    if (date.isNull() || !date.isValid()) { return false; }
    return RTC::setAlarm(date);
}

bool Manager::setDisplayBacklight(const QString &device, int value)
{
    qDebug() << "Try to set DISPLAY backlight" << device << value;
    if (!Common::canAdjustBacklight(device)) { return false; }
    int light = value;
    if (light>Common::backlightMax(device)) { light = Common::backlightMax(device); }
    else if (light<0) { light = 0; }
    return Common::adjustBacklight(device, light);
}

bool Manager::SetCpuGovernor(const QString &gov)
{
    qDebug() << "Try to set CPU governor" << gov;
    return Common::setCpuGovernor(gov);
}

bool Manager::SetCpuFrequency(const QString &freq)
{
    qDebug() << "Try to set CPU frequency" << freq;
    return  Common::setCpuFrequency(freq);
}

