/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWERKIT_BACKLIGHT_H
#define POWERKIT_BACKLIGHT_H

#include <QString>

namespace PowerKit
{
    class Backlight
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
}

#endif // POWERKIT_BACKLIGHT_H
