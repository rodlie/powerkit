/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef SYSTRAY_H
#define SYSTRAY_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QIcon>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QSettings>
#include <QPainter>
#include <QMap>
#include <QMapIterator>
#include <QProcess>
#include <QMap>
#include <QFileSystemWatcher>

#include "common.h"
#include "power.h"
#include "service.h"
#include "powermanagement.h"
#include "screensaver.h"
#include "screens.h"

#include <X11/extensions/scrnsaver.h>
#undef CursorShape
#undef Bool
#undef Status
#undef None
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef FontChange
#undef Expose
#undef FrameFeature
#undef Unsorted

#define XSCREENSAVER_RUN "xscreensaver -no-splash"

class SysTray : public QObject
{
    Q_OBJECT

public:
    explicit SysTray(QObject *parent = NULL);
    ~SysTray();

private:
    QSystemTrayIcon *tray;
    Power *man;
    PowerManagement *pm;
    ScreenSaver *ss;
    PowerDwarf *pd;
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
    bool desktopSS;
    bool desktopPM;
    bool showTray;
    bool disableLidOnExternalMonitors;
    int autoSuspendBatteryAction;
    int autoSuspendACAction;
    QProcess *xscreensaver;
    bool startupScreensaver;
    QMap<quint32,QString> ssInhibitors;
    QMap<quint32,QString> pmInhibitors;
    QString internalMonitor;
    QFileSystemWatcher *watcher;
    bool lidXrandr;

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
    void handleCritical();
    void drawBattery(double left);
    void timeout();
    int xIdle();
    void resetTimer();
    void setInternalMonitor();
    bool internalMonitorIsConnected();
    bool externalMonitorIsConnected();
    void handleNewInhibitScreenSaver(QString application,
                                     QString reason,
                                     quint32 cookie);
    void handleNewInhibitPowerManagement(QString application,
                                         QString reason,
                                         quint32 cookie);
    void handleDelInhibitScreenSaver(quint32 cookie);
    void handleDelInhibitPowerManagement(quint32 cookie);
    void handleScreensaverFinished(int exitcode);
    void showMessage(QString title, QString msg, bool critical = false);
    void handleConfChanged(QString file);
    void disableHibernate();
    void disableSuspend();
};

#endif // SYSTRAY_H
