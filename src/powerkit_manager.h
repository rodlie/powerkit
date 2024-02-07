/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWERKIT_MANAGER_H
#define POWERKIT_MANAGER_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QDBusInterface>
#include <QDBusObjectPath>
#include <QDBusMessage>
#include <QTimer>
#include <QDateTime>
#include <QDBusUnixFileDescriptor>

#include "powerkit_device.h"

namespace PowerKit
{
    class Manager : public QObject
    {
        Q_OBJECT

    public:
        explicit Manager(QObject *parent = 0);
        ~Manager();
        QMap<QString, Device*> getDevices();

    private:
        QMap<QString, Device*> devices;
        QMap<quint32,QString> ssInhibitors;
        QMap<quint32,QString> pmInhibitors;

        QDBusInterface *upower;
        QDBusInterface *logind;

        QTimer timer;

        bool wasDocked;
        bool wasLidClosed;
        bool wasOnBattery;

        QScopedPointer<QDBusUnixFileDescriptor> suspendLock;
        QScopedPointer<QDBusUnixFileDescriptor> lidLock;

    signals:
        void Update();
        void UpdatedDevices();
        void LidClosed();
        void LidOpened();
        void SwitchedToBattery();
        void SwitchedToAC();
        void PrepareForSuspend();
        void PrepareForResume();
        void DeviceWasRemoved(const QString &path);
        void DeviceWasAdded(const QString &path);
        void UpdatedInhibitors();
        void Error(const QString &message);
        void Warning(const QString &message);

    private slots:
        bool canLogind(const QString &method);
        const QDBusMessage callLogind(const QString &method);

        QStringList find();
        void setup();
        void check();
        void scan();

        void deviceAdded(const QDBusObjectPath &obj);
        void deviceAdded(const QString &path);
        void deviceRemoved(const QDBusObjectPath &obj);
        void deviceRemoved(const QString &path);
        void deviceChanged();
        void propertiesChanged();
        void handleDeviceChanged(const QString &device);
        void handlePrepareForSuspend(bool prepare);
        void clearDevices();
        void handleNewInhibitScreenSaver(const QString &application,
                                         const QString &reason,
                                         quint32 cookie);
        void handleNewInhibitPowerManagement(const QString &application,
                                             const QString &reason,
                                             quint32 cookie);
        void handleDelInhibitScreenSaver(quint32 cookie);
        void handleDelInhibitPowerManagement(quint32 cookie);

        bool registerSuspendLock();
        bool registerLidLock();

    public slots:
        bool HasSuspendLock();
        bool HasLidLock();

        bool CanRestart();
        bool CanPowerOff();
        bool CanSuspend();
        bool CanHibernate();
        bool CanHybridSleep();
        bool CanSuspendThenHibernate();

        const QString Restart();
        const QString PowerOff();
        const QString Suspend();
        const QString Hibernate();
        const QString HybridSleep();
        const QString SuspendThenHibernate();

        bool IsDocked();
        bool LidIsPresent();
        bool LidIsClosed();
        bool OnBattery();
        double BatteryLeft();
        void LockScreen();
        bool HasBattery();
        qlonglong TimeToEmpty();
        qlonglong TimeToFull();
        void UpdateDevices();
        void UpdateBattery();
        void UpdateConfig();
        const QStringList GetScreenSaverInhibitors();
        const QStringList GetPowerManagementInhibitors();
        QMap<quint32, QString> GetInhibitors();
        void ReleaseSuspendLock();
        void ReleaseLidLock();
        bool SetDisplayBacklight(QString const &device, int value);
    };
}

#endif // POWERKIT_MANAGER_H
