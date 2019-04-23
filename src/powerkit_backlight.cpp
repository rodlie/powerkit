/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit_backlight.h"
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QDebug>

const QString PowerBacklight::getDevice()
{
    QString path = "/sys/class/backlight";
    QDirIterator it(path, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString foundDir = it.next();
        if (foundDir.startsWith(QString("%1/radeon").arg(path))) {
            return foundDir;
        } else if (foundDir.startsWith(QString("%1/intel").arg(path))) {
            return foundDir;
        } else if (foundDir.startsWith(QString("%1/acpi").arg(path))) {
            return foundDir;
        }
    }
    return QString();
}

bool PowerBacklight::canAdjustBrightness(const QString &device)
{
    QFileInfo backlight(QString("%1/brightness").arg(device));
    if (backlight.isWritable()) { return true; }
    return false;
}

bool PowerBacklight::canAdjustBrightness()
{
    return canAdjustBrightness(getDevice());
}

int PowerBacklight::getMaxBrightness(const QString &device)
{
    int result = 0;
    QFile backlight(QString("%1/max_brightness").arg(device));
    if (backlight.open(QIODevice::ReadOnly)) {
        result = backlight.readAll().trimmed().toInt();
        backlight.close();
    }
    return result;
}

int PowerBacklight::getMaxBrightness()
{
    return getMaxBrightness(getDevice());
}

int PowerBacklight::getCurrentBrightness(const QString &device)
{
    int result = 0;
    QFile backlight(QString("%1/brightness").arg(device));
    if (backlight.open(QIODevice::ReadOnly)) {
        result = backlight.readAll().trimmed().toInt();
        backlight.close();
    }
    return result;
}

int PowerBacklight::getCurrentBrightness()
{
    return getCurrentBrightness(getDevice());
}

bool PowerBacklight::setCurrentBrightness(const QString &device, int value)
{
    if (!canAdjustBrightness(device)) { return false; }
    QFile backlight(QString("%1/brightness").arg(device));
    if (backlight.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        QTextStream out(&backlight);
        if (value<1) { value = 1; }
        out << QString::number(value);
        backlight.close();
        if (value == getCurrentBrightness(device)) { return true;}
    }
    return false;
}

bool PowerBacklight::setCurrentBrightness(int value)
{
    return setCurrentBrightness(getDevice(), value);
}
