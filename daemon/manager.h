/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QString>

class Manager : public QObject
{
    Q_OBJECT

public:
    explicit Manager(QObject *parent = NULL);

public slots:
    bool setWakeAlarm(const QString &alarm);
};

#endif // MANAGER_H
