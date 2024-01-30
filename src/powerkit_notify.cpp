/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018-2022 Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit_notify.h"
#include "powerkit_common.h"
#include "powerkit_manager.h"

using namespace PowerKit;

SystemNotification::SystemNotification(QObject* parent)
  : QObject(parent)
{
    dbus =
      new QDBusInterface(NOTIFY_SERVICE,
                         NOTIFY_PATH,
                         NOTIFY_SERVICE,
                         QDBusConnection::sessionBus(),
                         this);
    valid = dbus->isValid();
}

void SystemNotification::sendMessage(const QString& title,
                                     const QString& text,
                                     const bool critical)
{
    QList<QVariant> args;
    QVariantMap hintsMap;
    int timeout = 5000;

    if (critical) {
        hintsMap.insert("urgency", QVariant::fromValue(uchar(2)));
        timeout = 0;
    }

    args << (qAppName())                 // appname
         << static_cast<unsigned int>(0) // id
         << DEFAULT_NOTIFY_ICON          // icon
         << title                        // summary
         << text                         // body
         << QStringList()                // actions
         << hintsMap                     // hints
         << timeout;                     // timeout
    dbus->callWithArgumentList( QDBus::AutoDetect, QString("Notify"), args);
}
