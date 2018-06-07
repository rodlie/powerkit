/*
# Power Dwarf <powerdwarf.dracolinux.org>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "dialog.h"
#include <QVBoxLayout>
#include <QIcon>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QTabWidget>
#include "common.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QMessageBox>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , lidActionBattery(0)
    , lidActionAC(0)
    , criticalActionBattery(0)
    , lowBattery(0)
    , criticalBattery(0)
    , autoSleepBattery(0)
    , autoSleepAC(0)
    , desktopSS(0)
    , desktopPM(0)
    , showNotifications(0)
    , showBatteryPercent(0)
    , showSystemTray(0)
    , disableLidActionAC(0)
    , disableLidActionBattery(0)
    , autoSleepBatteryAction(0)
{
    // setup dialog
    setAttribute(Qt::WA_QuitOnClose, true);
    setWindowTitle(QString("Power Dwarf"));
    setWindowIcon(QIcon::fromTheme(DEFAULT_BATTERY_ICON, QIcon(":/icons/battery.png")));
    setMinimumSize(QSize(480,360));

    // setup widgets
    QVBoxLayout *layout = new QVBoxLayout(this);
    QTabWidget *containerWidget = new QTabWidget(this);

    QWidget *wrapper = new QWidget(this);
    QHBoxLayout *wrapperLayout = new QHBoxLayout(wrapper);
    layout->addWidget(wrapper);

    QWidget *powerContainer = new QWidget(this);
    powerContainer->setContentsMargins(0,0,0,0);
    QVBoxLayout *powerContainerLayout = new QVBoxLayout(powerContainer);
    powerContainerLayout->setMargin(0);
    powerContainerLayout->setSpacing(0);

    QLabel *powerLabel = new QLabel(this);
    QIcon powerIcon = QIcon::fromTheme("powerdwarf", QIcon(":/icons/powerdwarf.png"));
    powerLabel->setPixmap(powerIcon.pixmap(QSize(128, 128)));
    powerLabel->setMinimumSize(QSize(128, 128));
    powerLabel->setMaximumSize(QSize(128, 128));

    QLabel *powerBatteryLabel = new QLabel(this);
    QIcon powerIconBattery = QIcon(":/icons/battery128.png");
    powerBatteryLabel->setPixmap(powerIconBattery.pixmap(QSize(128, 128)));
    powerBatteryLabel->setMinimumSize(QSize(128, 128));
    powerBatteryLabel->setMaximumSize(QSize(128, 128));

    powerContainerLayout->addWidget(powerLabel);
    powerContainerLayout->addWidget(powerBatteryLabel);

    //layout->setSizeConstraint(QLayout::SetFixedSize); // lock dialog size
    wrapperLayout->addWidget(powerContainer);
    wrapperLayout->addWidget(containerWidget);

    QWidget *batteryContainer = new QWidget(this);
    QVBoxLayout *batteryContainerLayout = new QVBoxLayout(batteryContainer);
    batteryContainerLayout->setMargin(0);
    batteryContainerLayout->setSpacing(0);

    QWidget *lidActionBatteryContainer = new QWidget(this);
    QHBoxLayout *lidActionBatteryContainerLayout = new QHBoxLayout(lidActionBatteryContainer);
    lidActionBattery = new QComboBox(this);
    QLabel *lidActionBatteryLabel = new QLabel(this);

    disableLidActionBattery = new QCheckBox(this);
    disableLidActionBattery->setIcon(QIcon::fromTheme("video-display", QIcon(":/icons/video-display.png")));
    disableLidActionBattery->setStyleSheet("margin:10px; font-style: italic;");
    disableLidActionBattery->setText(tr("Disable if external monitor is connected."));

    QLabel *lidActionBatteryIcon = new QLabel(this);
    lidActionBatteryIcon->setMaximumSize(24,24);
    lidActionBatteryIcon->setMinimumSize(24,24);
    lidActionBatteryIcon->setPixmap(QIcon::fromTheme("video-display", QIcon(":/icons/video-display.png")).pixmap(QSize(24,24)));
    lidActionBatteryLabel->setText(tr("<strong>Lid Action</strong>"));
    lidActionBatteryContainerLayout->addWidget(lidActionBatteryIcon);
    lidActionBatteryContainerLayout->addWidget(lidActionBatteryLabel);
    lidActionBatteryContainerLayout->addWidget(lidActionBattery);
    batteryContainerLayout->addWidget(lidActionBatteryContainer);
    batteryContainerLayout->addWidget(disableLidActionBattery);

    QWidget *lowBatteryContainer = new QWidget(this);
    QHBoxLayout *lowBatteryContainerLayout = new QHBoxLayout(lowBatteryContainer);
    lowBattery = new QSpinBox(this);
    lowBattery->setMinimum(0);
    lowBattery->setMaximum(99);
    lowBattery->setSuffix(tr(" %"));
    QLabel *lowBatteryLabel = new QLabel(this);

    QLabel *lowBatteryIcon = new QLabel(this);
    lowBatteryIcon->setMaximumSize(24,24);
    lowBatteryIcon->setMinimumSize(24,24);
    lowBatteryIcon->setPixmap(QIcon::fromTheme("battery-low-charging", QIcon(":/icons/battery-low-charging.png")).pixmap(QSize(24,24)));
    lowBatteryLabel->setText(tr("<strong>Low battery</strong>"));
    lowBatteryContainerLayout->addWidget(lowBatteryIcon);
    lowBatteryContainerLayout->addWidget(lowBatteryLabel);
    lowBatteryContainerLayout->addWidget(lowBattery);
    batteryContainerLayout->addWidget(lowBatteryContainer);

    QWidget *criticalBatteryContainer = new QWidget(this);
    QHBoxLayout *criticalBatteryContainerLayout = new QHBoxLayout(criticalBatteryContainer);
    criticalBattery = new QSpinBox(this);
    criticalBattery->setMinimum(0);
    criticalBattery->setMaximum(99);
    criticalBattery->setSuffix(tr(" %"));
    QLabel *criticalBatteryLabel = new QLabel(this);

    QWidget *criticalActionBatteryContainer = new QWidget(this);
    criticalActionBatteryContainer->setContentsMargins(0,0,0,0);
    QVBoxLayout *criticalActionBatteryContainerLayout = new QVBoxLayout(criticalActionBatteryContainer);
    criticalActionBatteryContainerLayout->setMargin(0);
    criticalActionBatteryContainerLayout->setSpacing(0);
    criticalActionBattery = new QComboBox(this);
    criticalActionBatteryContainerLayout->addWidget(criticalBattery);
    criticalActionBatteryContainerLayout->addWidget(criticalActionBattery);

    QLabel *criticalBatteryIcon = new QLabel(this);
    criticalBatteryIcon->setMaximumSize(24,24);
    criticalBatteryIcon->setMinimumSize(24,24);
    criticalBatteryIcon->setPixmap(QIcon::fromTheme("battery-caution", QIcon(":/icons/battery-caution.png")).pixmap(QSize(24,24)));
    criticalBatteryLabel->setText(tr("<strong>Critical battery</strong>"));
    criticalBatteryContainerLayout->addWidget(criticalBatteryIcon);
    criticalBatteryContainerLayout->addWidget(criticalBatteryLabel);
    criticalBatteryContainerLayout->addWidget(criticalActionBatteryContainer);
    batteryContainerLayout->addWidget(criticalBatteryContainer);

    QWidget *sleepBatteryContainer = new QWidget(this);
    QHBoxLayout *sleepBatteryContainerLayout = new QHBoxLayout(sleepBatteryContainer);
    autoSleepBattery = new QSpinBox(this);
    autoSleepBattery->setMinimum(0);
    autoSleepBattery->setMaximum(1000);
    autoSleepBattery->setSuffix(tr(" min"));
    QLabel *sleepBatteryLabel = new QLabel(this);

    QWidget *sleepActionBatteryContainer = new QWidget(this);
    sleepActionBatteryContainer->setContentsMargins(0,0,0,0);
    QVBoxLayout *sleepActionBatteryContainerLayout = new QVBoxLayout(sleepActionBatteryContainer);
    sleepActionBatteryContainerLayout->setMargin(0);
    sleepActionBatteryContainerLayout->setSpacing(0);
    autoSleepBatteryAction = new QComboBox(this);
    sleepActionBatteryContainerLayout->addWidget(autoSleepBattery);
    sleepActionBatteryContainerLayout->addWidget(autoSleepBatteryAction);

    QLabel *sleepBatteryIcon = new QLabel(this);
    sleepBatteryIcon->setMaximumSize(24,24);
    sleepBatteryIcon->setMinimumSize(24,24);
    sleepBatteryIcon->setPixmap(QIcon::fromTheme("system-suspend", QIcon(":/icons/system-suspend.png")).pixmap(QSize(24,24)));
    sleepBatteryLabel->setText(tr("<strong>Suspend after</strong>"));
    sleepBatteryContainerLayout->addWidget(sleepBatteryIcon);
    sleepBatteryContainerLayout->addWidget(sleepBatteryLabel);
    sleepBatteryContainerLayout->addWidget(sleepActionBatteryContainer);
    batteryContainerLayout->addWidget(sleepBatteryContainer);

    batteryContainerLayout->addStretch();
    containerWidget->addTab(batteryContainer, QIcon::fromTheme("battery", QIcon(":/icons/battery.png")), tr("On Battery"));

    QWidget *acContainer = new QWidget(this);
    QVBoxLayout *acContainerLayout = new QVBoxLayout(acContainer);
    acContainerLayout->setMargin(0);
    acContainerLayout->setSpacing(0);

    QWidget *lidActionACContainer = new QWidget(this);
    QHBoxLayout *lidActionACContainerLayout = new QHBoxLayout(lidActionACContainer);
    lidActionAC = new QComboBox(this);
    QLabel *lidActionACLabel = new QLabel(this);

    disableLidActionAC = new QCheckBox(this);
    disableLidActionAC->setStyleSheet("margin: 10px; font-style: italic;");
    disableLidActionAC->setText(tr("Disable if external monitor is connected."));

    lidActionACLabel->setText(tr("<strong>Lid Action</strong>"));
    lidActionACContainerLayout->addWidget(lidActionACLabel);
    lidActionACContainerLayout->addWidget(lidActionAC);
    acContainerLayout->addWidget(lidActionACContainer);
    acContainerLayout->addWidget(disableLidActionAC);

    QWidget *sleepACContainer = new QWidget(this);
    QHBoxLayout *sleepACContainerLayout = new QHBoxLayout(sleepACContainer);
    autoSleepAC = new QSpinBox(this);
    autoSleepAC->setMinimum(0);
    autoSleepAC->setMaximum(1000);
    autoSleepAC->setSuffix(tr(" min"));
    QLabel *sleepACLabel = new QLabel(this);

    sleepACLabel->setText(tr("<strong>Suspend after</strong>"));
    sleepACContainerLayout->addWidget(sleepACLabel);
    sleepACContainerLayout->addWidget(autoSleepAC);
    acContainerLayout->addWidget(sleepACContainer);

    acContainerLayout->addStretch();
    containerWidget->addTab(acContainer, QIcon::fromTheme("ac-adapter", QIcon(":/icons/ac-adapter.png")), tr("On AC"));

    QWidget *desktopContainer = new QWidget(this);
    QVBoxLayout *desktopContainerLayout = new QVBoxLayout(desktopContainer);

    desktopSS = new QCheckBox(this);
    desktopSS->setText("org.freedesktop.ScreenSaver");
    desktopPM = new QCheckBox(this);
    desktopPM->setText("org.freedesktop.PowerManagement");

    desktopContainerLayout->addWidget(desktopSS);
    desktopContainerLayout->addWidget(desktopPM);

    desktopContainerLayout->addStretch();
    containerWidget->addTab(desktopContainer, QIcon::fromTheme("user-desktop", QIcon(":/icons/user-desktop.png")), tr("Desktop Integration"));

    QWidget *extraContainer = new QWidget(this);
    QHBoxLayout *extraContainerLayout = new QHBoxLayout(extraContainer);
    extraContainerLayout->addStretch();

    showSystemTray  = new QCheckBox(this);
    showSystemTray->setIcon(QIcon::fromTheme("preferences-other", QIcon(":/icons/preferences-other.png")));
    showSystemTray->setText(tr("Show System tray"));
    extraContainerLayout->addWidget(showSystemTray);

    showNotifications = new QCheckBox(this);
    showNotifications->setIcon(QIcon::fromTheme("user-available", QIcon(":/icons/user-available.png")));
    showNotifications->setText(tr("Show Notifications"));
    extraContainerLayout->addWidget(showNotifications);

    showBatteryPercent = new QCheckBox(this);
    showBatteryPercent->setStyleSheet("margin: 10px;");
    showBatteryPercent->setIcon(QIcon::fromTheme("battery", QIcon(":/icons/battery.png")));
    showBatteryPercent->setText(tr("Show Battery percent in system tray."));
    batteryContainerLayout->addWidget(showBatteryPercent);

    layout->addWidget(extraContainer);

    populate(); // populate boxes
    loadSettings(); // load settings

    // connect varius widgets
    connect(lidActionBattery, SIGNAL(currentIndexChanged(int)), this, SLOT(handleLidActionBattery(int)));
    connect(lidActionAC, SIGNAL(currentIndexChanged(int)), this, SLOT(handleLidActionAC(int)));
    connect(criticalActionBattery, SIGNAL(currentIndexChanged(int)), this, SLOT(handleCriticalAction(int)));
    connect(lowBattery, SIGNAL(valueChanged(int)), this, SLOT(handleLowBattery(int)));
    connect(criticalBattery, SIGNAL(valueChanged(int)), this, SLOT(handleCriticalBattery(int)));
    connect(autoSleepBattery, SIGNAL(valueChanged(int)), this, SLOT(handleAutoSleepBattery(int)));
    connect(autoSleepAC, SIGNAL(valueChanged(int)), this, SLOT(handleAutoSleepAC(int)));
    connect(desktopSS, SIGNAL(toggled(bool)), this, SLOT(handleDesktopSS(bool)));
    connect(desktopPM, SIGNAL(toggled(bool)), this, SLOT(handleDesktopPM(bool)));
    connect(showNotifications, SIGNAL(toggled(bool)), this, SLOT(handleShowNotifications(bool)));
    connect(showBatteryPercent, SIGNAL(toggled(bool)), this, SLOT(handleShowBatteryPercent(bool)));
    connect(showSystemTray, SIGNAL(toggled(bool)), this, SLOT(handleShowSystemTray(bool)));
    connect(disableLidActionAC, SIGNAL(toggled(bool)), this, SLOT(handleDisableLidActionAC(bool)));
    connect(disableLidActionBattery, SIGNAL(toggled(bool)), this, SLOT(handleDisableLidActionBattery(bool)));
    connect(autoSleepBatteryAction, SIGNAL(currentIndexChanged(int)), this, SLOT(handleAutoSleepBatteryAction(int)));
}

// populate widgets with default values
void Dialog::populate()
{
    lidActionBattery->clear();
    lidActionBattery->addItem(tr("None"), lidNone);
    lidActionBattery->addItem(tr("Lock Screen"), lidLock);
    lidActionBattery->addItem(tr("Sleep"), lidSleep);
    lidActionBattery->addItem(tr("Hibernate"), lidHibernate);

    lidActionAC->clear();
    lidActionAC->addItem(tr("None"), lidNone);
    lidActionAC->addItem(tr("Lock Screen"), lidLock);
    lidActionAC->addItem(tr("Sleep"), lidSleep);
    lidActionAC->addItem(tr("Hibernate"), lidHibernate);

    criticalActionBattery->clear();
    criticalActionBattery->addItem(tr("None"), criticalNone);
    criticalActionBattery->addItem(tr("Hibernate"), criticalHibernate);
    criticalActionBattery->addItem(tr("Shutdown"), criticalShutdown);

    autoSleepBatteryAction->clear();
    autoSleepBatteryAction->addItem(tr("None"), suspendNone);
    autoSleepBatteryAction->addItem(tr("Sleep"), suspendSleep);
    autoSleepBatteryAction->addItem(tr("Hibernate"), suspendHibernate);
    autoSleepBatteryAction->addItem(tr("Shutdown"), suspendShutdown);
}

// load settings and set as default in widgets
void Dialog::loadSettings()
{
    int defaultAutoSleepBattery = AUTO_SLEEP_BATTERY;
    if (Common::validPowerSettings("suspend_battery_timeout")) {
        defaultAutoSleepBattery = Common::loadPowerSettings("suspend_battery_timeout").toInt();
    }
    setDefaultAction(autoSleepBattery, defaultAutoSleepBattery);

    int defaultAutoSleepBatteryAction = DEFAULT_SUSPEND_BATTERY_ACTION;
    if (Common::validPowerSettings("suspend_battery_action")) {
        defaultAutoSleepBatteryAction = Common::loadPowerSettings("suspend_battery_action").toInt();
    }
    setDefaultAction(autoSleepBatteryAction, defaultAutoSleepBatteryAction);

    int defaultAutoSleepAC = 0; // don't add default on AC
    if (Common::validPowerSettings("suspend_ac_timeout")) {
        defaultAutoSleepAC = Common::loadPowerSettings("suspend_ac_timeout").toInt();
    }
    setDefaultAction(autoSleepAC, defaultAutoSleepAC);

    int defaultLowBattery = LOW_BATTERY;
    if (Common::validPowerSettings("lowBattery")) {
        defaultLowBattery = Common::loadPowerSettings("lowBattery").toInt();
    }
    setDefaultAction(lowBattery, defaultLowBattery);

    int defaultCriticalBattery = CRITICAL_BATTERY;
    if (Common::validPowerSettings("criticalBattery")) {
        defaultCriticalBattery = Common::loadPowerSettings("criticalBattery").toInt();
    }
    setDefaultAction(criticalBattery, defaultCriticalBattery);

    int defaultLidActionBattery = LID_BATTERY_DEFAULT;
    if (Common::validPowerSettings("lidBattery")) {
        defaultLidActionBattery = Common::loadPowerSettings("lidBattery").toInt();
    }
    setDefaultAction(lidActionBattery, defaultLidActionBattery);

    int defaultLidActionAC = LID_AC_DEFAULT;
    if (Common::validPowerSettings("lidAC")) {
        defaultLidActionAC = Common::loadPowerSettings("lidAC").toInt();
    }
    setDefaultAction(lidActionAC, defaultLidActionAC);

    int defaultCriticalAction = CRITICAL_DEFAULT;
    if (Common::validPowerSettings("criticalAction")) {
        defaultCriticalAction = Common::loadPowerSettings("criticalAction").toInt();
    }
    setDefaultAction(criticalActionBattery, defaultCriticalAction);

    bool defaultDesktopSS = true;
    if (Common::validPowerSettings("desktop_ss")) {
        defaultDesktopSS = Common::loadPowerSettings("desktop_ss").toBool();
    }
    desktopSS->setChecked(defaultDesktopSS);

    bool defaultDesktopPM = true;
    if (Common::validPowerSettings("desktop_pm")) {
        defaultDesktopPM = Common::loadPowerSettings("desktop_pm").toBool();
    }
    desktopPM->setChecked(defaultDesktopPM);

    bool defaultShowNotifications = true;
    if (Common::validPowerSettings("tray_notify")) {
        defaultShowNotifications = Common::loadPowerSettings("tray_notify").toBool();
    }
    showNotifications->setChecked(defaultShowNotifications);

    bool defaultShowBatteryPercent = true;
    if (Common::validPowerSettings("show_battery_percent")) {
        defaultShowBatteryPercent = Common::loadPowerSettings("show_battery_percent").toBool();
    }
    showBatteryPercent->setChecked(defaultShowBatteryPercent);

    bool defaultShowTray = true;
    if (Common::validPowerSettings("show_tray")) {
        defaultShowTray = Common::loadPowerSettings("show_tray").toBool();
    }
    showSystemTray->setChecked(defaultShowTray);

    bool defaultDisableLidActionBattery = true;
    if (Common::validPowerSettings("disable_lid_action_battery_external_monitor")) {
        defaultDisableLidActionBattery = Common::loadPowerSettings("disable_lid_action_battery_external_monitor").toBool();
    }
    disableLidActionBattery->setChecked(defaultDisableLidActionBattery);

    bool defaultDisableLidActionAC = true;
    if (Common::validPowerSettings("disable_lid_action_ac_external_monitor")) {
        defaultDisableLidActionAC = Common::loadPowerSettings("disable_lid_action_ac_external_monitor").toBool();
    }
    disableLidActionAC->setChecked(defaultDisableLidActionAC);
}

// tell power manager to update settings
void Dialog::updatePM()
{
    QDBusInterface iface(PM_SERVICE, PM_PATH, PM_SERVICE, QDBusConnection::sessionBus());
    if (!iface.isValid()) { return; }
    iface.call("refresh");
}

// set default action in combobox
void Dialog::setDefaultAction(QComboBox *box, int action)
{
    for (int i=0;i<box->count();i++) {
        if (box->itemData(i).toInt() == action) {
            box->setCurrentIndex(i);
            return;
        }
    }
}

// set default value in spinbox
void Dialog::setDefaultAction(QSpinBox *box, int action)
{
    box->setValue(action);
}

// save current value and update power manager
void Dialog::handleLidActionBattery(int index)
{
    Common::savePowerSettings("lidBattery", index);
    updatePM();
}

void Dialog::handleLidActionAC(int index)
{
    Common::savePowerSettings("lidAC", index);
    updatePM();
}

void Dialog::handleCriticalAction(int index)
{
    Common::savePowerSettings("criticalAction", index);
    updatePM();
}

void Dialog::handleLowBattery(int value)
{
    Common::savePowerSettings("lowBattery", value);
    updatePM();
}

void Dialog::handleCriticalBattery(int value)
{
    Common::savePowerSettings("criticalBattery", value);
    updatePM();
}

void Dialog::handleAutoSleepBattery(int value)
{
    Common::savePowerSettings("suspend_battery_timeout", value);
    updatePM();
}

void Dialog::handleAutoSleepAC(int value)
{
    Common::savePowerSettings("suspend_ac_timeout", value);
    updatePM();
}

void Dialog::handleDesktopSS(bool triggered)
{
    Common::savePowerSettings("desktop_ss", triggered);
    updatePM();
    QMessageBox::information(this, tr("Restart required"), tr("You must restart the power daemon to apply this setting"));
}

void Dialog::handleDesktopPM(bool triggered)
{
    Common::savePowerSettings("desktop_pm", triggered);
    updatePM();
    QMessageBox::information(this, tr("Restart required"), tr("You must restart the power daemon to apply this setting"));
}

void Dialog::handleShowNotifications(bool triggered)
{
    Common::savePowerSettings("tray_notify", triggered);
    updatePM();
}

void Dialog::handleShowBatteryPercent(bool triggered)
{
    Common::savePowerSettings("show_battery_percent", triggered);
    updatePM();
}

void Dialog::handleShowSystemTray(bool triggered)
{
    Common::savePowerSettings("show_tray", triggered);
    updatePM();
}

void Dialog::handleDisableLidActionAC(bool triggered)
{
    Common::savePowerSettings("disable_lid_action_ac_external_monitor", triggered);
    updatePM();
}

void Dialog::handleDisableLidActionBattery(bool triggered)
{
    Common::savePowerSettings("disable_lid_action_battery_external_monitor", triggered);
    updatePM();
}

void Dialog::handleAutoSleepBatteryAction(int index)
{
    Common::savePowerSettings("suspend_battery_action", index);
    updatePM();
}
