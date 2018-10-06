/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "service.h"

PowerDwarf::PowerDwarf(QObject *parent)
    : QObject(parent)
{

}

void PowerDwarf::handleNewInhibitScreenSaver(QString application, QString reason, quint32 cookie)
{
    Q_UNUSED(reason)
    ssInhibitors[cookie] = application;
    emit InhibitorsSSChanged();
}

void PowerDwarf::handleNewInhibitPowerManagement(QString application, QString reason, quint32 cookie)
{
    Q_UNUSED(reason)
    pmInhibitors[cookie] = application;
    emit InhibitorsPMChanged();
}

void PowerDwarf::handleDelInhibitScreenSaver(quint32 cookie)
{
    if (ssInhibitors.contains(cookie)) {
        ssInhibitors.remove(cookie);
        emit InhibitorsSSChanged();
    }
}

void PowerDwarf::handleDelInhibitPowerManagement(quint32 cookie)
{
    if (pmInhibitors.contains(cookie)) {
        pmInhibitors.remove(cookie);
        emit InhibitorsPMChanged();
    }
}

void PowerDwarf::Refresh()
{
    emit update();
}

QStringList PowerDwarf::InhibitorsSS()
{
    QStringList result;
    QMapIterator<quint32, QString> i(ssInhibitors);
    while (i.hasNext()) { i.next(); result << i.value(); }
    return result;
}

QStringList PowerDwarf::InhibitorsPM()
{
    QStringList result;
    QMapIterator<quint32, QString> i(pmInhibitors);
    while (i.hasNext()) { i.next(); result << i.value(); }
    return result;
}
