/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018-2022 Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWER_SETTINGS_H
#define POWER_SETTINGS_H

#include <QVariant>
#include <QString>

class PowerSettings
{
public:
    static void setValue(const QString &type, const QVariant &value);
    static const QVariant getValue(const QString &type,
                                   const QVariant &fallback = QVariant());
    static bool isValid(const QString &type);
    static void saveDefault();
    static const QString getConf();
    static const QString getDir();
};

#endif // POWER_SETTINGS_H
