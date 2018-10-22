/*
# PowerKit <https://github.com/rodlie/powerkit>
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
#include <QLabel>
#include <QPixmap>
#include <QTabWidget>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QMessageBox>
#include <QPushButton>
#include <QApplication>
#include <QProcess>
#include <QDebug>
#include <QSlider>
#include <QFileSystemWatcher>
#include <QGroupBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QProgressBar>
#include <QMap>
#include <QLCDNumber>
#include <QDateTime>
#include <QScrollArea>

#include "def.h"
#include "common.h"
#include "powerkit.h"

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

#define DEVICE_UUID Qt::UserRole+1
#define DEVICE_TYPE Qt::UserRole+2
#define MAX_WIDTH 150

class Dialog : public QDialog
{
    Q_OBJECT

public:
   explicit Dialog(QWidget *parent = NULL);
   ~Dialog();

private:
    QDBusInterface *dbus;
    QComboBox *lidActionBattery;
    QComboBox *lidActionAC;
    QComboBox *criticalActionBattery;
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
    //QCheckBox *lidXrandr;
    QString backlightDevice;
    bool hasBacklight;
    QSlider *backlightSlider;
    QFileSystemWatcher *backlightWatcher;
    PowerKit *man;
    QLabel *batteryIcon;
    QLabel *batteryLabel;
    QTreeWidget *deviceTree;
    QMap<QString,QProgressBar*> devicesProg;
    QLCDNumber *batteryLeftLCD;
    QSlider *backlightSliderBattery;
    QSlider *backlightSliderAC;
    QCheckBox *backlightBatteryCheck;
    QCheckBox *backlightACCheck;
    QCheckBox *backlightBatteryLowerCheck;
    QCheckBox *backlightACHigherCheck;
    QTreeWidget *inhibitorTree;

private slots:
    void populate();
    void loadSettings();
    void saveSettings();
    void setDefaultAction(QComboBox *box, int action);
    void setDefaultAction(QSpinBox *box, int action);
    void setDefaultAction(QComboBox *box, QString value);
    void handleLidActionBattery(int index);
    void handleLidActionAC(int index);
    void handleCriticalAction(int index);
    void handleCriticalBattery(int value);
    void handleAutoSleepBattery(int value);
    void handleAutoSleepAC(int value);
    void handleDesktopSS(bool triggered);
    void handleDesktopPM(bool triggered);
    //void handleLidXrandr(bool triggered);
    void handleShowNotifications(bool triggered);
    void handleShowSystemTray(bool triggered);
    void handleDisableLidAction(bool triggered);
    void handleAutoSleepBatteryAction(int index);
    void handleAutoSleepACAction(int index);
    void handleLockscreenButton();
    void handleSleepButton();
    void handleHibernateButton();
    void handlePoweroffButton();
    void checkPerms();
    void handleBacklightSlider(int value);
    void updateBacklight(QString file);
    void checkDevices();
    bool deviceExists(QString uid);
    void deviceRemove(QString uid);
    void handleDeviceAdded(QString uid);
    void handleBacklightBatteryCheck(bool triggered);
    void handleBacklightACCheck(bool triggered);
    void handleBacklightBatterySlider(int value);
    void handleBacklightACSlider(int value);
    void hibernateWarn();
    void sleepWarn();
    void handleBacklightBatteryCheckLower(bool triggered);
    void handleBacklightACCheckHigher(bool triggered);
    void handleUpdatedInhibitors();
    void getInhibitors();
    void enableBacklight(bool enabled);
};

#endif // DIALOG_H
