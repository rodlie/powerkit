/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018-2022 Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWER_BACKLIGHT_H
#define POWER_BACKLIGHT_H

#include <QString>

class PowerBacklight
{
public:
    static const QString getDevice();
    static bool canAdjustBrightness(const QString &device);
    static bool canAdjustBrightness();
    static int getMaxBrightness(const QString &device);
    static int getMaxBrightness();
    static int getCurrentBrightness(const QString &device);
    static int getCurrentBrightness();
    static bool setCurrentBrightness(const QString &device, int value);
    static bool setCurrentBrightness(int value);
};

#endif // POWER_BACKLIGHT_H
