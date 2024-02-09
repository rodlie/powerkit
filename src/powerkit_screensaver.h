/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWERKIT_SCREENSAVER_H
#define POWERKIT_SCREENSAVER_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QMap>
#include <QString>

namespace PowerKit
{
    class ScreenSaver : public QObject
    {
        Q_OBJECT

    public:
        explicit ScreenSaver(QObject *parent = NULL);

    private:
        QTimer timer;
        QMap<quint32, QTime> clients;
        int blank;
        QString xlock;

    signals:
        void newInhibit(const QString &application_name,
                        const QString &reason_for_inhibit,
                        quint32 cookie);
        void removedInhibit(quint32 cookie);

    private slots:
        quint32 genCookie();
        void checkForExpiredClients();
        bool canInhibit();
        void timeOut();

    public slots:
        void Update();
        void Lock();
        void SimulateUserActivity();
        quint32 GetSessionIdleTime();
        quint32 Inhibit(const QString &application_name,
                        const QString &reason_for_inhibit);
        void UnInhibit(quint32 cookie);
        static const QMap<QString, bool> GetDisplays();
        static const QString GetInternalDisplay();
        static void setDisplaysOff(bool off);
    };
}

#endif // POWERKIT_SCREENSAVER_H
