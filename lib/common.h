/*
# PowerDwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef COMMON_H
#define COMMON_H

#include <QVariant>

class Common
{
public:
    static void savePowerSettings(QString type, QVariant value);
    static QVariant loadPowerSettings(QString type);
    static bool validPowerSettings(QString type);
    static QString vendor();
    static bool isDarkTheme();
    static void setDarkTheme();
    static void setNormalTheme();
    static void setIconTheme();
};

#endif // COMMON_H
