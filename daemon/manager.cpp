/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "manager.h"
#include "rtc.h"

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

