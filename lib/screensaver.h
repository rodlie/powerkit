/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QMap>

class ScreenSaver : public QObject
{
    Q_OBJECT

public:
    explicit ScreenSaver();

private:
    QTimer timer;
    QMap<quint32, QTime> clients;

signals:
    void newInhibit(QString application, QString reason, quint32 cookie);
    void removedInhibit(quint32 cookie);

private slots:
    int randInt(int low, int high);
    quint32 genCookie();
    void checkForExpiredClients();
    bool canInhibit();
    void checkForDBusSession();
    void timeOut();
    void pingPM();

public slots:
    void SimulateUserActivity();
    quint32 Inhibit(QString application, QString reason);
    void UnInhibit(quint32 cookie);
};

#endif // SCREENSAVER_H
