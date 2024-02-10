/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit_backlight.h"
#include "powerkit_common.h"

#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>

#define LOGIND_SESSION "org.freedesktop.login1.Session"
#define LOGIND_PATH_SESSION "/org/freedesktop/login1/session/self"

using namespace PowerKit;

const QString Backlight::getDevice()
{
    QString path = "/sys/class/backlight";
    QDirIterator it(path, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString foundDir = it.next();
        if (foundDir.startsWith(QString("%1/radeon").arg(path))) {
            return foundDir;
        } else if (foundDir.startsWith(QString("%1/amdgpu").arg(path))) {
            return foundDir;
        } else if (foundDir.startsWith(QString("%1/intel").arg(path))) {
            return foundDir;
        } else if (foundDir.startsWith(QString("%1/acpi").arg(path))) {
            return foundDir;
        }
    }
    return QString();
}

bool Backlight::canAdjustBrightness(const QString &device)
{
    QFileInfo backlight(QString("%1/brightness").arg(device));
    if (backlight.isWritable()) { return true; }
    return false;
}

bool Backlight::canAdjustBrightness()
{
    return canAdjustBrightness(getDevice());
}

int Backlight::getMaxBrightness(const QString &device)
{
    int result = 0;
    QFile backlight(QString("%1/max_brightness").arg(device));
    if (backlight.open(QIODevice::ReadOnly)) {
        result = backlight.readAll().trimmed().toInt();
        backlight.close();
    }
    return result;
}

int Backlight::getMaxBrightness()
{
    return getMaxBrightness(getDevice());
}

int Backlight::getCurrentBrightness(const QString &device)
{
    int result = 0;
    QFile backlight(QString("%1/brightness").arg(device));
    if (backlight.open(QIODevice::ReadOnly)) {
        result = backlight.readAll().trimmed().toInt();
        backlight.close();
    }
    return result;
}

int Backlight::getCurrentBrightness()
{
    return getCurrentBrightness(getDevice());
}

bool Backlight::setCurrentBrightness(const QString &device, int value)
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

bool Backlight::setCurrentBrightness(int value)
{
    return setCurrentBrightness(getDevice(), value);
}

bool Backlight::setBrightness(const QString &device,
                              int value)
{
    QDBusInterface iface(POWERKIT_LOGIND_SERVICE,
                         LOGIND_PATH_SESSION,
                         LOGIND_SESSION,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    QDBusMessage reply = iface.call("SetBrightness",
                                    "backlight",
                                    device.split("/").takeLast(),
                                    (quint32)value);
    return reply.errorMessage().isEmpty();
}

bool Backlight::setBrightness(int value)
{
    return setBrightness(getDevice(), value);
}
