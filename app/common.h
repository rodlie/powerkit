/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef COMMON_H
#define COMMON_H

#include <QVariant>
#include <QString>

class Common
{
public:
    static void savePowerSettings(QString type, QVariant value);
    static QVariant loadPowerSettings(QString type);
    static bool validPowerSettings(QString type);
    static void saveDefaultSettings();
    static void setIconTheme();
    static QString confFile();
    static QString confDir();
    static bool kernelCanResume(bool ignore = false /* if ignore then always return true */);
    static QString backlightDevice();
    static bool canAdjustBacklight(QString device);
    static int backlightMax(QString device);
    static int backlightValue(QString device);
    static bool adjustBacklight(QString device, int value);
    static void checkSettings();
};

#endif // COMMON_H
