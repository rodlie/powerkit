/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018-2022 Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWERKIT_NOTIFY_H
#define POWERKIT_NOTIFY_H

#include <QObject>
#include <QApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QVariant>

class QDBusInterface;

namespace PowerKit
{
    class SystemNotification : public QObject
    {
        Q_OBJECT

    public:
        explicit SystemNotification(QObject* parent = nullptr);
        bool valid;

        void sendMessage(const QString& title,
                         const QString& text,
                         const bool critical);

    private:
        QDBusInterface *dbus;
    };
}

#endif // POWERKIT_NOTIFY_H
