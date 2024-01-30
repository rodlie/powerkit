/*
# PowerKit <https://github.com/rodlie/powerkit>
# CCopyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
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
    return PowerKit::Rtc::setAlarm(date);
}

bool Manager::SetDisplayBacklight(const QString &device, int value)
{
    qDebug() << "Try to set DISPLAY backlight" << device << value;
    if (!PowerKit::Backlight::canAdjustBrightness(device)) { return false; }
    int light = value;
    if (light>PowerKit::Backlight::getMaxBrightness(device)) { light = PowerKit::Backlight::getMaxBrightness(device); }
    else if (light<0) { light = 0; }
    return PowerKit::Backlight::setCurrentBrightness(device, light);
}

bool Manager::SetCpuGovernor(const QString &gov)
{
    qDebug() << "Try to set CPU governor" << gov;
    return PowerKit::Cpu::setGovernor(gov);
}

bool Manager::SetCpuFrequency(const QString &freq)
{
    qDebug() << "Try to set CPU frequency" << freq;
    return  PowerKit::Cpu::setFrequency(freq);
}

bool Manager::SetPStateTurbo(bool enable)
{
    qDebug() << "Try to set Intel Turbo Boost" << enable;
    return PowerKit::Cpu::setPStateTurbo(enable);
}

bool Manager::SetPStateMax(int value)
{
    qDebug() << "Try to set Intel pstate max" << value;
    return PowerKit::Cpu::setPStateMax(value);
}

bool Manager::SetPStateMin(int value)
{
    qDebug() << "Try to set Intel pstate min" << value;
    return PowerKit::Cpu::setPStateMin(value);
}

bool Manager::SetPState(int min, int max)
{
    qDebug() << "Try to set Intel pstate min max" << min << max;
    return PowerKit::Cpu::setPState(min, max);
}
