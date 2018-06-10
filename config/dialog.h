/*
# Power Dwarf <powerdwarf.dracolinux.org>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QIcon>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QTabWidget>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QMessageBox>
#include <QPushButton>
#include <QProcess>
#include <QTimer>

#include "common.h"
#include "upower.h"
#include "monitor.h"
// fix X11 inc
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

#ifndef XSCREENSAVER_LOCK
#define XSCREENSAVER_LOCK "xscreensaver-command -lock"
#endif

class Dialog : public QDialog
{
    Q_OBJECT

public:
   explicit Dialog(QWidget *parent = NULL);

signals:
    void refresh();

private:
    QDBusInterface *dbus;
    QComboBox *lidActionBattery;
    QComboBox *lidActionAC;
    QComboBox *criticalActionBattery;
    QSpinBox *lowBattery;
    QSpinBox *criticalBattery;
    QSpinBox *autoSleepBattery;
    QSpinBox *autoSleepAC;
    QCheckBox *desktopSS;
    QCheckBox *desktopPM;
    QCheckBox *showNotifications;
    QCheckBox *showBatteryPercent;
    QCheckBox *showSystemTray;
    QCheckBox *disableLidActionAC;
    QCheckBox *disableLidActionBattery;
    QComboBox *autoSleepBatteryAction;
    QPushButton *lockscreenButton;
    QPushButton *sleepButton;
    QPushButton *hibernateButton;
    QPushButton *poweroffButton;

private slots:
    void populate();
    void loadSettings();
    void updatePM();
    void setDefaultAction(QComboBox *box, int action);
    void setDefaultAction(QSpinBox *box, int action);
    void handleLidActionBattery(int index);
    void handleLidActionAC(int index);
    void handleCriticalAction(int index);
    void handleLowBattery(int value);
    void handleCriticalBattery(int value);
    void handleAutoSleepBattery(int value);
    void handleAutoSleepAC(int value);
    void handleDesktopSS(bool triggered);
    void handleDesktopPM(bool triggered);
    void handleShowNotifications(bool triggered);
    void handleShowBatteryPercent(bool triggered);
    void handleShowSystemTray(bool triggered);
    void handleDisableLidActionAC(bool triggered);
    void handleDisableLidActionBattery(bool triggered);
    void handleAutoSleepBatteryAction(int index);
    void handleUpdatedMonitors();
    void handleLockscreenButton();
    void handleSleepButton();
    void handleHibernateButton();
    void handlePoweroffButton();
};

#endif // DIALOG_H
