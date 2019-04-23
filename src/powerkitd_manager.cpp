/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkitd_manager.h"
#include "powerkit_rtc.h"
#include "powerkit_backlight.h"
#include "powerkit_cpu.h"

#include <QDebug>

Manager::Manager(QObject *parent) : QObject(parent)
{
}

bool Manager::SetWakeAlarm(const QString &alarm)
{
    qDebug() << "Try to set RTC wake alarm" << alarm;
    QDateTime date = QDateTime::fromString(alarm, "yyyy-MM-dd HH:mm:ss");
    if (date.isNull() || !date.isValid()) { return false; }
    return PowerRtc::setAlarm(date);
}

bool Manager::SetDisplayBacklight(const QString &device, int value)
{
    qDebug() << "Try to set DISPLAY backlight" << device << value;
    if (!PowerBacklight::canAdjustBrightness(device)) { return false; }
    int light = value;
    if (light>PowerBacklight::getMaxBrightness(device)) { light = PowerBacklight::getMaxBrightness(device); }
    else if (light<0) { light = 0; }
    return PowerBacklight::setCurrentBrightness(device, light);
}

bool Manager::SetCpuGovernor(const QString &gov)
{
    qDebug() << "Try to set CPU governor" << gov;
    return PowerCpu::setGovernor(gov);
}

bool Manager::SetCpuFrequency(const QString &freq)
{
    qDebug() << "Try to set CPU frequency" << freq;
    return  PowerCpu::setFrequency(freq);
}

