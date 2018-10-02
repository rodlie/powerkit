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
    explicit PowerDwarf(){}

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
                                     quint32 cookie)
    {
        Q_UNUSED(reason)
        ssInhibitors[cookie] = application;
        emit InhibitorsSSChanged();
    }
    void handleNewInhibitPowerManagement(QString application,
                                         QString reason,
                                         quint32 cookie)
    {
        Q_UNUSED(reason)
        pmInhibitors[cookie] = application;
        emit InhibitorsPMChanged();
    }
    void handleDelInhibitScreenSaver(quint32 cookie)
    {
        if (ssInhibitors.contains(cookie)) {
            ssInhibitors.remove(cookie);
            emit InhibitorsSSChanged();
        }
    }
    void handleDelInhibitPowerManagement(quint32 cookie)
    {
        if (pmInhibitors.contains(cookie)) {
            pmInhibitors.remove(cookie);
            emit InhibitorsPMChanged();
        }
    }

public slots:
    void Refresh()
    {
        emit update();
    }
    QStringList InhibitorsSS()
    {
        QStringList result;
        QMapIterator<quint32, QString> i(ssInhibitors);
        while (i.hasNext()) { i.next(); result << i.value(); }
        return result;
    }
    QStringList InhibitorsPM()
    {
        QStringList result;
        QMapIterator<quint32, QString> i(pmInhibitors);
        while (i.hasNext()) { i.next(); result << i.value(); }
        return result;
    }
};

#endif // POWERDWARF_SERVICE_H
