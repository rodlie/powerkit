/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#
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
#include <QListWidget>
#include <QListWidgetItem>
#include <QApplication>

#include "def.h"
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

class Dialog : public QDialog
{
    Q_OBJECT

public:
   explicit Dialog(QWidget *parent = NULL);
   ~Dialog();

signals:
    void refresh();

private:
    QDBusInterface *dbus;
    QComboBox *lidActionBattery;
    QComboBox *lidActionAC;
    QComboBox *criticalActionBattery;
    //QSpinBox *lowBattery;
    QSpinBox *criticalBattery;
    QSpinBox *autoSleepBattery;
    QSpinBox *autoSleepAC;
    QCheckBox *desktopSS;
    QCheckBox *desktopPM;
    QCheckBox *showNotifications;
    QCheckBox *showSystemTray;
    QCheckBox *disableLidAction;
    QComboBox *autoSleepBatteryAction;
    QComboBox *autoSleepACAction;
    QPushButton *lockscreenButton;
    QPushButton *sleepButton;
    QPushButton *hibernateButton;
    QPushButton *poweroffButton;
    //QComboBox *lowBatteryAction;
#ifdef USE_XRANDR
    QListWidget *monitorList;
    QComboBox *monitorModes;
    QComboBox *monitorRates;
    QCheckBox *monitorPrimary;
    monitorInfo currentMonitorInfo;
    QPushButton *monitorSaveButton;
    QPushButton *monitorApplyButton;
    QComboBox *monitorRotation;
    QComboBox *monitorPosition;
    QComboBox *monitorPositionOther;
#endif

private slots:
    void populate();
    void loadSettings();
    void updatePM();
    void setDefaultAction(QComboBox *box, int action);
    void setDefaultAction(QSpinBox *box, int action);
    void setDefaultAction(QComboBox *box, QString value);
#ifdef USE_XRANDR
    void setDefaultRotation(QString value);
#endif
    void handleLidActionBattery(int index);
    void handleLidActionAC(int index);
    void handleCriticalAction(int index);
    //void handleLowBattery(int value);
    void handleCriticalBattery(int value);
    void handleAutoSleepBattery(int value);
    void handleAutoSleepAC(int value);
    void handleDesktopSS(bool triggered);
    void handleDesktopPM(bool triggered);
    void handleShowNotifications(bool triggered);
    void handleShowSystemTray(bool triggered);
    void handleDisableLidAction(bool triggered);
    void handleAutoSleepBatteryAction(int index);
    void handleAutoSleepACAction(int index);
    void handleUpdatedMonitors();
    void handleLockscreenButton();
    void handleSleepButton();
    void handleHibernateButton();
    void handlePoweroffButton();
    //void handleLowBatteryAction(int value);
#ifdef USE_XRANDR
    bool monitorExists(QString display);
    void handleMonitorListItemChanged(QListWidgetItem *item);
    void handleMonitorListICurrentitemChanged(QListWidgetItem *item,
                                              QListWidgetItem *item2);
    void handleMonitorModeChanged(QString mode);
    void monitorSaveSettings();
    void monitorApplySettings();
#endif
};

#endif // DIALOG_H
