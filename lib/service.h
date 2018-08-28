/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#
*/

#ifndef POWERDWARF_SERVICE_H
#define POWERDWARF_SERVICE_H

#include <QObject>

class PowerDwarf : public QObject
{
    Q_OBJECT

public:
    explicit PowerDwarf(){}

signals:
    void updatedMonitors();
    void update();

private slots:
    void updateMonitors()
    {
        emit updatedMonitors();
    }

public slots:
    void refresh()
    {
        emit update();
    }
};

#endif // POWERDWARF_SERVICE_H
