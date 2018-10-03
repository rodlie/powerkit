/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWERDWARF_SERVICE_H
#define POWERDWARF_SERVICE_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QMapIterator>

class PowerDwarf : public QObject
{
    Q_OBJECT

public:
    explicit PowerDwarf(QObject *parent = NULL);

private:
    QMap<quint32,QString> ssInhibitors;
    QMap<quint32,QString> pmInhibitors;

signals:
    void update();
    void InhibitorsSSChanged();
    void InhibitorsPMChanged();

private slots:
    void handleNewInhibitScreenSaver(QString application,
                                     QString reason,
                                     quint32 cookie);
    void handleNewInhibitPowerManagement(QString application,
                                         QString reason,
                                         quint32 cookie);
    void handleDelInhibitScreenSaver(quint32 cookie);
    void handleDelInhibitPowerManagement(quint32 cookie);

public slots:
    void Refresh();
    QStringList InhibitorsSS();
    QStringList InhibitorsPM();
};

#endif // POWERDWARF_SERVICE_H
