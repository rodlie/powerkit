/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWERKIT_APP_H
#define POWERKIT_APP_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QProcess>
#include <QFileSystemWatcher>
#include <QEvent>
#include <QWheelEvent>

#include "powerkit_powermanagement.h"
#include "powerkit_screensaver.h"
#include "powerkit_manager.h"

namespace PowerKit
{
    class TrayIcon : public QSystemTrayIcon
    {
        Q_OBJECT
    public:
        enum WheelAction {
            WheelUp,
            WheelDown
        };
        TrayIcon(QObject *parent = 0)
            : QSystemTrayIcon(parent), wheel_delta(0) {}
        TrayIcon(const QIcon &icon, QObject *parent = 0)
            : QSystemTrayIcon(icon, parent), wheel_delta(0) {}
        bool event(QEvent *event);
    signals:
        void wheel(TrayIcon::WheelAction action);
    private:
        int wheel_delta;
    };

    class App : public QObject
    {
        Q_OBJECT

    public:
        explicit App(QObject *parent = NULL);
        ~App();

    private:
        TrayIcon *tray;
        PowerKit::Manager *man;
        PowerKit::PowerManagement *pm;
        PowerKit::ScreenSaver *ss;
        bool wasLowBattery;
        bool wasVeryLowBattery;
        int lowBatteryValue;
        int critBatteryValue;
        bool hasService;
        int lidActionBattery;
        int lidActionAC;
        int criticalAction;
        int autoSuspendBattery;
        int autoSuspendAC;
        QTimer *timer;
        int timeouts;
        bool showNotifications;
        bool showTray;
        bool disableLidOnExternalMonitors;
        int autoSuspendBatteryAction;
        int autoSuspendACAction;
        QString internalMonitor;
        QFileSystemWatcher *watcher;
        bool lidXrandr;
        bool lidWasClosed;
        QString backlightDevice;
        bool hasBacklight;
        bool backlightOnBattery;
        bool backlightOnAC;
        int backlightBatteryValue;
        int backlightACValue;
        bool backlightBatteryDisableIfLower;
        bool backlightACDisableIfHigher;
        bool warnOnLowBattery;
        bool warnOnVeryLowBattery;
        bool notifyOnBattery;
        bool notifyOnAC;
        bool notifyNewInhibitor;
        bool backlightMouseWheel;
        bool ignoreKernelResume;

    private slots:
        void trayActivated(QSystemTrayIcon::ActivationReason reason);
        void checkDevices();
        void handleClosedLid();
        void handleOpenedLid();
        void handleOnBattery();
        void handleOnAC();
        void loadSettings();
        void registerService();
        void handleHasInhibitChanged(bool has_inhibit);
        void handleLow(double left);
        void handleVeryLow(double left);
        void handleCritical(double left);
        void drawBattery(double left);
        void updateToolTip();
        void timeout();
        void resetTimer();
        void setInternalMonitor();
        bool internalMonitorIsConnected();
        bool externalMonitorIsConnected();
        void handleNewInhibitScreenSaver(const QString &application,
                                         const QString &reason,
                                         quint32 cookie);
        void handleNewInhibitPowerManagement(const QString &application,
                                             const QString &reason,
                                             quint32 cookie);
        void handleDelInhibitScreenSaver(quint32 cookie);
        void handleDelInhibitPowerManagement(quint32 cookie);
        void showMessage(const QString &title,
                         const QString &msg,
                         bool critical = false);
        void handleConfChanged(const QString &file);
        void disableHibernate();
        void disableSuspend();
        void handlePrepareForSuspend();
        void handlePrepareForResume();
        void switchInternalMonitor(bool toggle);
        void handleTrayWheel(TrayIcon::WheelAction action);
        void handleDeviceChanged(const QString &path);
        void openSettings();
        void handleError(const QString &message);
        void handleWarning(const QString &message);
    };
}

#endif // POWERKIT_APP_H
