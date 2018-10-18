/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWERMANAGEMENT_H
#define POWERMANAGEMENT_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QTime>
#include <QString>

class PowerManagement : public QObject
{
    Q_OBJECT

public:
    explicit PowerManagement(QObject *parent = NULL);

private:
    QTimer timer;
    QMap<quint32, QTime> clients;

signals:
    void HasInhibitChanged(bool has_inhibit);
    void newInhibit(QString application, QString reason, quint32 cookie);
    void removedInhibit(quint32 cookie);

private slots:
    int randInt(int low, int high);
    quint32 genCookie();
    void checkForExpiredClients();
    bool canInhibit();
    void timeOut();

public slots:
    void SimulateUserActivity();
    quint32 Inhibit(QString application, QString reason);
    void UnInhibit(quint32 cookie);
    bool HasInhibit();
};

#endif // POWERMANAGEMENT_H
