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
        enum PKBackend {
            PKNoBackend,
            PKConsoleKit,
            PKLogind,
            PKUPower
        };

        enum PKAction {
            PKNoAction,
            PKRestartAction,
            PKPowerOffAction,
            PKSuspendAction,
            PKHibernateAction,
            PKHybridSleepAction
        };

        enum PKMethod {
            PKNoMethod,
            PKCanRestart,
            PKCanPowerOff,
            PKCanSuspend,
            PKCanHibernate,
            PKCanHybridSleep,
            PKSuspendAllowed,
            PKHibernateAllowed
        };

        explicit Manager(QObject *parent = 0);
        ~Manager();
        QMap<QString, Device*> getDevices();

    private:
        QMap<QString, Device*> devices;
        QMap<quint32,QString> ssInhibitors;
        QMap<quint32,QString> pmInhibitors;

        QDBusInterface *upower;
        QDBusInterface *logind;
        QDBusInterface *pmd;

        QTimer timer;

        bool wasDocked;
        bool wasLidClosed;
        bool wasOnBattery;

        bool wakeAlarm;
        QDateTime wakeAlarmDate;

        QScopedPointer<QDBusUnixFileDescriptor> suspendLock;

        int suspendWakeupBattery;
        int suspendWakeupAC;

        bool lockScreenOnSuspend;
        bool lockScreenOnResume;

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

    private slots:
        bool availableService(const QString &service,
                              const QString &path,
                              const QString &interface);
        bool availableAction(const PKMethod &method,
                             const PKBackend &backend);
        QString executeAction(const PKAction &action,
                              const PKBackend &backend);

        QStringList find();
        void setup();
        void check();
        void scan();

        void deviceAdded(const QDBusObjectPath &obj);
        void deviceAdded(const QString &path);
        void deviceRemoved(const QDBusObjectPath &obj);
        void deviceRemoved(const QString &path);
        void deviceChanged();
        void handleDeviceChanged(const QString &device);
        void handleResume();
        void handleSuspend();
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
        void SetWakeAlarmFromSettings();

    public slots:
        bool HasLogind();
        bool HasUPower();
        bool HasPowerKitd();
        bool HasWakeAlarm();
        bool HasSuspendLock();

        bool CanRestart();
        bool CanPowerOff();
        bool CanSuspend();
        bool CanHibernate();
        bool CanHybridSleep();

        const QString Restart();
        const QString PowerOff();
        const QString Suspend();
        const QString Hibernate();
        const QString HybridSleep();
        bool SetWakeAlarm(const QDateTime &date);
        void ClearWakeAlarm();

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
        const QDateTime GetWakeAlarm();
        void ReleaseSuspendLock();
        void SetSuspendWakeAlarmOnBattery(int value);
        void SetSuspendWakeAlarmOnAC(int value);
        bool SetDisplayBacklight(QString const &device, int value);
        bool SetPState(int min, int max);
        bool SetPStateMin(int value);
        bool SetPStateMax(int value);
    };
}

#endif // POWERKIT_MANAGER_H
