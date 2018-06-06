/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWERMANAGEMENT_H
#define POWERMANAGEMENT_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QTime>
#include <QMap>
#include <QMapIterator>
#include <QDBusConnection>
#include <QCoreApplication>
#include <QDebug>

#define PM_SERVICE "org.freedesktop.PowerManagement"
#define TIMEOUT 60000
#define MAX_INHIBIT 18000

class PowerManagement : public QObject
{
    Q_OBJECT

public:
    explicit PowerManagement()
    {
        timer.setInterval(TIMEOUT);
        connect(&timer, SIGNAL(timeout()), this, SLOT(timeOut()));
        timer.start();
    }

private:
    QTimer timer;
    QMap<quint32, QTime> clients;

signals:
    void HasInhibitChanged(bool has_inhibit);
    void update();

private slots:
    int randInt(int low, int high)
    {
        QTime time = QTime::currentTime();
        qsrand((uint)time.msec());
        return qrand() % ((high + 1) - low) + low;
    }
    quint32 genCookie()
    {
        int low = 0;
        int high = 1000;
        quint32 cookie = randInt(low, high);
        while(!clients.contains(cookie)) {
            if (!clients.contains(cookie)) { clients[cookie] = QTime::currentTime(); }
            else { cookie = randInt(low, high); }
        }
        return cookie;
    }
    void checkForExpiredClients()
    {
        QMapIterator<quint32, QTime> client(clients);
        while (client.hasNext()) {
            client.next();
            if (client.value().secsTo(QTime::currentTime())>=MAX_INHIBIT) { clients.remove(client.key()); }
        }
    }
    bool canInhibit()
    {
        checkForExpiredClients();
        if (clients.size()>0) { return true; }
        return false;
    }
    void timeOut()
    {
        if (canInhibit()) { SimulateUserActivity(); }
    }

public slots:
    void SimulateUserActivity()
    {
        emit HasInhibitChanged(true);
    }
    quint32 Inhibit(QString /*application*/, QString /*reason*/)
    {
        quint32 cookie = genCookie();
        timeOut();
        emit HasInhibitChanged(canInhibit());
        return cookie;
    }
    void UnInhibit(quint32 cookie)
    {
        if (clients.contains(cookie)) { clients.remove(cookie); }
        timeOut();
        emit HasInhibitChanged(canInhibit());
    }
    bool HasInhibit()
    {
        return canInhibit();
    }
    void refresh()
    {
        emit update();
    }
};

#endif // POWERMANAGEMENT_H
