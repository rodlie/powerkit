/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWERKIT_POWERMANAGEMENT_H
#define POWERKIT_POWERMANAGEMENT_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QTime>
#include <QString>

namespace PowerKit
{
    class PowerManagement : public QObject
    {
        Q_OBJECT

    public:
        explicit PowerManagement(QObject *parent = NULL);

    private:
        QTimer timer;
        QMap<quint32, QTime> clients;

    signals:
        void HasInhibitChanged(bool has_inhibit_changed);
        void newInhibit(const QString &application,
                        const QString &reason,
                        quint32 cookie);
        void removedInhibit(quint32 cookie);

    private slots:
        quint32 genCookie();
        void checkForExpiredClients();
        bool canInhibit();
        void timeOut();

    public slots:
        void SimulateUserActivity();
        quint32 Inhibit(const QString &application,
                        const QString &reason);
        void UnInhibit(quint32 cookie);
        bool HasInhibit();
    };
}

#endif // POWERKIT_POWERMANAGEMENT_H
