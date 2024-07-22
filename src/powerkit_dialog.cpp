/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit_dialog.h"
#include "powerkit_common.h"
#include "powerkit_theme.h"
#include "powerkit_settings.h"
#include "powerkit_backlight.h"
#include "powerkit_client.h"
#include "powerkit_cpu.h"

#include <QTimer>

#define MAX_WIDTH 175

using namespace PowerKit;

Dialog::Dialog(QWidget *parent,
               bool quitOnClose)
    : QDialog(parent)
    , cpuTimer(nullptr)
    , dbus(nullptr)
    , lidActionBattery(nullptr)
    , lidActionAC(nullptr)
    , criticalActionBattery(nullptr)
    , criticalBattery(nullptr)
    , autoSleepBattery(nullptr)
    , autoSleepAC(nullptr)
    , showNotifications(nullptr)
    , showSystemTray(nullptr)
    , disableLidAction(nullptr)
    , autoSleepBatteryAction(nullptr)
    , autoSleepACAction(nullptr)
    , backlightSliderBattery(nullptr)
    , backlightSliderAC(nullptr)
    , backlightBatteryCheck(nullptr)
    , backlightACCheck(nullptr)
    , backlightBatteryLowerCheck(nullptr)
    , backlightACHigherCheck(nullptr)
    , warnOnLowBattery(nullptr)
    , warnOnVeryLowBattery(nullptr)
    , lidActionACLabel(nullptr)
    , lidActionBatteryLabel(nullptr)
    , batteryBacklightLabel(nullptr)
    , acBacklightLabel(nullptr)
    , backlightMouseWheel(nullptr)
    , batteryStatusLabel(nullptr)
    , cpuFreqLabel(nullptr)
    , cpuTempLabel(nullptr)
    , screensaverBlank(nullptr)
    , screensaverLock(nullptr)
    , hasCpuCoreTemp(false)
    , hasBattery(false)
{
    // setup dialog
    if (quitOnClose) { setAttribute(Qt::WA_QuitOnClose, true); }
    setWindowTitle(tr("Power Manager"));
    setMinimumSize(QSize(680, 360));

    // setup cpu timer
    cpuTimer = new QTimer(this);
    cpuTimer->setInterval(1000);
    connect(cpuTimer,
            SIGNAL(timeout()),
            this,
            SLOT(drawCpu()));
    cpuTimer->start();

    // setup dbus
    QDBusConnection session = QDBusConnection::sessionBus();
    dbus = new QDBusInterface(POWERKIT_SERVICE,
                              POWERKIT_PATH,
                              POWERKIT_MANAGER,
                              session, this);
    if (!dbus->isValid()) {
        QMessageBox::warning(this,
                             tr("powerkit not running"),
                             tr("powerkit is not running, please start powerkit before running this application."));
        QTimer::singleShot(100, qApp, SLOT(quit()));
        return;
    }

    // detect device changes
    connect(dbus, SIGNAL(UpdatedDevices()),
            this, SLOT(handleUpdatedDevices()));

    // trigger generation of powerkit.conf if not exists
    Settings::getConf();

    setupWidgets();
    populateWidgets();
    loadSettings();
    connectWidgets();
    handleUpdatedDevices();
}

Dialog::~Dialog()
{
    const auto lastGeo = Settings::getValue(CONF_DIALOG).toByteArray();
    const auto newGeo = saveGeometry();
    if (lastGeo != newGeo) {
        Settings::setValue(CONF_DIALOG, newGeo);
    }
}

void Dialog::setupWidgets()
{
    // setup theme
    Theme::setAppTheme();
    Theme::setIconTheme();
    setStyleSheet("QLabel { font-weight: bold; }");
    setWindowIcon(QIcon::fromTheme(POWERKIT_ICON));

    // setup widgets
    const auto layout = new QVBoxLayout(this);
    layout->setMargin(5);
    layout->setSpacing(0);

    // status widgets
    const auto statusWidget = new QWidget(this);
    statusWidget->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Fixed);
    const auto statusLayout = new QHBoxLayout(statusWidget);

    batteryStatusLabel = new QLabel(this);

    int iconLabelSize = 64;
    batteryStatusLabel->setMaximumSize(iconLabelSize, iconLabelSize);
    batteryStatusLabel->setMinimumSize(iconLabelSize, iconLabelSize);

    cpuFreqLabel = new QLabel(this);
    cpuFreqLabel->setMaximumSize(iconLabelSize, iconLabelSize);
    cpuFreqLabel->setMinimumSize(iconLabelSize, iconLabelSize);

    cpuTempLabel = new QLabel(this);
    cpuTempLabel->setMaximumSize(iconLabelSize, iconLabelSize);
    cpuTempLabel->setMinimumSize(iconLabelSize, iconLabelSize);

    statusLayout->addWidget(batteryStatusLabel);
    statusLayout->addWidget(cpuFreqLabel);
    statusLayout->addWidget(cpuTempLabel);

    // battery
    const auto batteryContainer = new QGroupBox(this);
    batteryContainer->setTitle(tr("On Battery"));
    batteryContainer->setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Expanding);
    const auto batteryContainerLayout = new QVBoxLayout(batteryContainer);
    batteryContainerLayout->setSpacing(0);

    const auto lidActionBatteryContainer = new QWidget(this);
    const auto lidActionBatteryContainerLayout = new QHBoxLayout(lidActionBatteryContainer);
    lidActionBatteryContainerLayout->setMargin(0);

    lidActionBattery = new QComboBox(this);
    lidActionBattery->setMaximumWidth(MAX_WIDTH);
    lidActionBattery->setMinimumWidth(MAX_WIDTH);

    lidActionBatteryLabel = new QLabel(this);
    lidActionBatteryLabel->setText(tr("Lid action"));
    lidActionBatteryLabel->setToolTip(tr("What to do when the lid is closed and running on battery."));

    lidActionBatteryContainerLayout->addWidget(lidActionBatteryLabel);
    lidActionBatteryContainerLayout->addStretch();
    lidActionBatteryContainerLayout->addWidget(lidActionBattery);

    const auto criticalBatteryContainer = new QWidget(this);
    const auto criticalBatteryContainerLayout = new QHBoxLayout(criticalBatteryContainer);
    criticalBatteryContainerLayout->setMargin(0);

    criticalBattery = new QSpinBox(this);
    criticalBattery->setMaximumWidth(MAX_WIDTH);
    criticalBattery->setMinimumWidth(MAX_WIDTH);
    criticalBattery->setMinimum(0);
    criticalBattery->setMaximum(99);
    criticalBattery->setSuffix(tr(" %"));
    criticalBattery->setPrefix(tr("At "));

    const auto criticalBatteryLabel = new QLabel(this);
    const auto criticalActionBatteryContainer = new QWidget(this);
    criticalActionBatteryContainer->setContentsMargins(0, 0, 0, 0);

    const auto criticalActionBatteryContainerLayout = new QVBoxLayout(criticalActionBatteryContainer);
    criticalActionBatteryContainerLayout->setMargin(0);

    criticalActionBatteryContainerLayout->setMargin(0);
    criticalActionBatteryContainerLayout->setSpacing(0);
    criticalActionBattery = new QComboBox(this);
    criticalActionBattery->setMaximumWidth(MAX_WIDTH);
    criticalActionBattery->setMinimumWidth(MAX_WIDTH);
    criticalActionBatteryContainerLayout->addWidget(criticalBattery);
    criticalActionBatteryContainerLayout->addWidget(criticalActionBattery);

    criticalBatteryLabel->setText(tr("Critical Action"));
    criticalBatteryLabel->setToolTip(tr("What to do when your battery is critical."));
    criticalBatteryContainerLayout->addWidget(criticalBatteryLabel);
    criticalBatteryContainerLayout->addStretch();
    criticalBatteryContainerLayout->addWidget(criticalActionBatteryContainer);

    const auto sleepBatteryContainer = new QWidget(this);
    const auto sleepBatteryContainerLayout = new QHBoxLayout(sleepBatteryContainer);
    sleepBatteryContainerLayout->setMargin(0);

    autoSleepBattery = new QSpinBox(this);
    autoSleepBattery->setMaximumWidth(MAX_WIDTH);
    autoSleepBattery->setMinimumWidth(MAX_WIDTH);
    autoSleepBattery->setMinimum(0);
    autoSleepBattery->setMaximum(1000);
    autoSleepBattery->setSuffix(QString(" %1").arg(tr("min")));
    autoSleepBattery->setPrefix(QString("%1 ").arg(tr("After")));

    const auto sleepBatteryLabel = new QLabel(this);
    const auto sleepActionBatteryContainer = new QWidget(this);
    const auto sleepActionBatteryContainerLayout = new QVBoxLayout(sleepActionBatteryContainer);

    sleepActionBatteryContainer->setContentsMargins(0, 0, 0, 0);
    sleepActionBatteryContainerLayout->setMargin(0);
    sleepActionBatteryContainerLayout->setSpacing(0);
    autoSleepBatteryAction = new QComboBox(this);
    autoSleepBatteryAction->setMaximumWidth(MAX_WIDTH);
    autoSleepBatteryAction->setMinimumWidth(MAX_WIDTH);
    sleepActionBatteryContainerLayout->addWidget(autoSleepBattery);
    sleepActionBatteryContainerLayout->addWidget(autoSleepBatteryAction);

    sleepBatteryLabel->setText(tr("Suspend Action"));
    sleepBatteryLabel->setToolTip(tr("Enable automatically suspend when on battery."));
    sleepBatteryContainerLayout->addWidget(sleepBatteryLabel);
    sleepBatteryContainerLayout->addStretch();
    sleepBatteryContainerLayout->addWidget(sleepActionBatteryContainer);

    // backlight battery
    backlightSliderBattery = new QSlider(this);
    backlightSliderBattery->setOrientation(Qt::Horizontal);
    backlightSliderBattery->setMinimum(1);
    backlightSliderBattery->setMaximum(1);
    backlightSliderBattery->setValue(0);

    backlightBatteryCheck = new QCheckBox(this);
    backlightBatteryCheck->setCheckable(true);
    backlightBatteryCheck->setChecked(false);
    backlightBatteryCheck->setToolTip(tr("Enable/Disable brightness override on battery."));
    backlightBatteryCheck->setText(QString(" ")); // qt ui bug workaround

    backlightBatteryLowerCheck = new QCheckBox(this);
    backlightBatteryLowerCheck->setCheckable(true);
    backlightBatteryLowerCheck->setChecked(false);
    backlightBatteryLowerCheck->setText(tr("Don't apply if lower"));
    backlightBatteryLowerCheck->setToolTip(tr("If your current brightness value is lower"
                                              " do not apply a brightness override (on battery)."));

    const auto backlightSliderBatteryContainer = new QWidget(this);
    const auto backlightSliderBatteryLayout = new QHBoxLayout(backlightSliderBatteryContainer);
    backlightSliderBatteryContainer->setContentsMargins(0, 0, 0, 0);
    backlightSliderBatteryLayout->setMargin(0);

    backlightSliderBatteryLayout->addWidget(backlightBatteryCheck);
    backlightSliderBatteryLayout->addWidget(backlightSliderBattery);

    const auto batteryBacklightOptContainer = new QWidget(this);
    const auto batteryBacklightOptContainerLayout = new QVBoxLayout(batteryBacklightOptContainer);

    batteryBacklightOptContainer->setContentsMargins(0, 0, 0, 0);
    batteryBacklightOptContainer->setMaximumWidth(MAX_WIDTH);
    batteryBacklightOptContainer->setMinimumWidth(MAX_WIDTH);
    batteryBacklightOptContainerLayout->setMargin(0);
    batteryBacklightOptContainerLayout->setContentsMargins(0, 0, 0, 0);
    batteryBacklightOptContainerLayout->addWidget(backlightSliderBatteryContainer);
    batteryBacklightOptContainerLayout->addWidget(backlightBatteryLowerCheck);

    const auto batteryBacklightContainer = new QWidget(this);
    const auto batteryBacklightContainerLayout = new QHBoxLayout(batteryBacklightContainer);
    batteryBacklightContainerLayout->setMargin(0);

    batteryBacklightLabel = new QLabel(this);
    batteryBacklightLabel->setText(tr("Brightness"));
    batteryBacklightLabel->setToolTip(tr("Override brightness when switched to battery power."));

    batteryBacklightContainerLayout->addWidget(batteryBacklightLabel);
    batteryBacklightContainerLayout->addStretch();
    batteryBacklightContainerLayout->addWidget(batteryBacklightOptContainer);

    // add battery widgets to container
    batteryContainerLayout->addWidget(lidActionBatteryContainer);
    batteryContainerLayout->addSpacing(10);
    batteryContainerLayout->addWidget(sleepBatteryContainer);
    batteryContainerLayout->addSpacing(10);
    batteryContainerLayout->addWidget(criticalBatteryContainer);
    batteryContainerLayout->addSpacing(10);
    batteryContainerLayout->addStretch();
    batteryContainerLayout->addWidget(batteryBacklightContainer);

    // AC
    const auto acContainer = new QGroupBox(this);
    acContainer->setTitle(tr("On AC"));
    const auto acContainerLayout = new QVBoxLayout(acContainer);
    acContainerLayout->setSpacing(0);

    const auto lidActionACContainer = new QWidget(this);
    const auto lidActionACContainerLayout = new QHBoxLayout(lidActionACContainer);
    lidActionACContainerLayout->setMargin(0);

    lidActionAC = new QComboBox(this);
    lidActionAC->setMaximumWidth(MAX_WIDTH);
    lidActionAC->setMinimumWidth(MAX_WIDTH);
    lidActionACLabel = new QLabel(this);

    lidActionACLabel->setText(tr("Lid action"));
    lidActionACLabel->setToolTip(tr("What to do when the lid is closed and running on AC."));
    lidActionACContainerLayout->addWidget(lidActionACLabel);
    lidActionACContainerLayout->addStretch();
    lidActionACContainerLayout->addWidget(lidActionAC);
    acContainerLayout->addWidget(lidActionACContainer);

    const auto sleepACContainer = new QWidget(this);
    const auto sleepACContainerLayout = new QHBoxLayout(sleepACContainer);
    sleepACContainerLayout->setMargin(0);

    autoSleepAC = new QSpinBox(this);
    autoSleepAC->setMaximumWidth(MAX_WIDTH);
    autoSleepAC->setMinimumWidth(MAX_WIDTH);
    autoSleepAC->setMinimum(0);
    autoSleepAC->setMaximum(1000);
    autoSleepAC->setSuffix(QString(" %1").arg(tr("min")));
    autoSleepAC->setPrefix(QString("%1 ").arg(tr("After")));

    const auto sleepACLabel = new QLabel(this);

    sleepACContainerLayout->addWidget(sleepACLabel);
    acContainerLayout->addWidget(sleepACContainer);

    const auto sleepActionACContainer = new QWidget(this);
    const auto sleepActionACContainerLayout = new QVBoxLayout(sleepActionACContainer);

    sleepActionACContainer->setContentsMargins(0, 0, 0, 0);
    sleepActionACContainerLayout->setMargin(0);
    sleepActionACContainerLayout->setSpacing(0);
    autoSleepACAction = new QComboBox(this);
    autoSleepACAction->setMaximumWidth(MAX_WIDTH);
    autoSleepACAction->setMinimumWidth(MAX_WIDTH);
    sleepActionACContainerLayout->addWidget(autoSleepAC);
    sleepActionACContainerLayout->addWidget(autoSleepACAction);

    sleepACLabel->setText(tr("Suspend Action"));
    sleepACLabel->setToolTip(tr("Enable automatically suspend when on AC."));
    sleepACContainerLayout->addWidget(sleepACLabel);
    sleepACContainerLayout->addStretch();
    sleepACContainerLayout->addWidget(sleepActionACContainer);

    // backlight ac
    backlightSliderAC = new QSlider(this);
    backlightSliderAC->setOrientation(Qt::Horizontal);
    backlightSliderAC->setMinimum(1);
    backlightSliderAC->setMaximum(1);
    backlightSliderAC->setValue(0);

    backlightACCheck = new QCheckBox(this);
    backlightACCheck->setCheckable(true);
    backlightACCheck->setChecked(false);
    backlightACCheck->setToolTip(tr("Enable/Disable brightness override on AC."));
    backlightACCheck->setText(QString(" ")); // qt ui bug workaround

    backlightACHigherCheck = new QCheckBox(this);
    backlightACHigherCheck->setCheckable(true);
    backlightACHigherCheck->setChecked(false);
    backlightACHigherCheck->setText(tr("Don't apply if higher"));
    backlightACHigherCheck->setToolTip(tr("If your current brightness value is higher"
                                          " do not apply a brightness override (on AC)."));

    const auto backlightSliderAcContainer = new QWidget(this);
    const auto backlightSliderAcLayout = new QHBoxLayout(backlightSliderAcContainer);
    backlightSliderAcContainer->setContentsMargins(0, 0, 0, 0);
    backlightSliderAcLayout->setMargin(0);

    backlightSliderAcLayout->addWidget(backlightACCheck);
    backlightSliderAcLayout->addWidget(backlightSliderAC);

    const auto acBacklightOptContainer = new QWidget(this);
    const auto acBacklightOptContainerLayout = new QVBoxLayout(acBacklightOptContainer);

    acBacklightOptContainer->setContentsMargins(0, 0, 0, 0);
    acBacklightOptContainer->setMaximumWidth(MAX_WIDTH);
    acBacklightOptContainer->setMinimumWidth(MAX_WIDTH);
    acBacklightOptContainerLayout->setMargin(0);
    acBacklightOptContainerLayout->setContentsMargins(0, 0, 0, 0);
    acBacklightOptContainerLayout->addWidget(backlightSliderAcContainer);
    acBacklightOptContainerLayout->addWidget(backlightACHigherCheck);

    const auto acBacklightContainer = new QWidget(this);
    const auto acBacklightContainerLayout = new QHBoxLayout(acBacklightContainer);
    acBacklightContainerLayout->setMargin(0);

    acBacklightLabel = new QLabel(this);

    acBacklightLabel->setText(tr("Brightness"));
    acBacklightLabel->setToolTip(tr("Override brightness when switched to AC power."));
    acBacklightContainerLayout->addWidget(acBacklightLabel);
    acBacklightContainerLayout->addStretch();
    acBacklightContainerLayout->addWidget(acBacklightOptContainer);

    // add widgets to ac
    acContainerLayout->addSpacing(10);
    acContainerLayout->addWidget(sleepACContainer);
    acContainerLayout->addSpacing(10);
    acContainerLayout->addStretch();
    acContainerLayout->addWidget(acBacklightContainer);

    // common
    const auto daemonContainer = new QGroupBox(this);
    daemonContainer->setTitle(tr("Options"));
    const auto daemonContainerLayout = new QVBoxLayout(daemonContainer);

    showSystemTray  = new QCheckBox(this);
    showSystemTray->setText(tr("Show system tray"));
    showSystemTray->setToolTip(tr("Enable/Disable the system tray icon."
                                  " Note that notifications will not work when the systemtray is disabled."));

    disableLidAction = new QCheckBox(this);
    disableLidAction->setText(tr("Disable lid action if external monitor connected"));
    disableLidAction->setToolTip(tr("Disable lid action if an external monitor is connected"
                                    " to your laptop."));

    backlightMouseWheel = new QCheckBox(this);
    backlightMouseWheel->setText(tr("Adjust brightness in system tray"));
    backlightMouseWheel->setToolTip(tr("Adjust the display backlight brightness with the mouse wheel on the system tray icon."));

    daemonContainerLayout->addWidget(showSystemTray);
    daemonContainerLayout->addWidget(backlightMouseWheel);
    daemonContainerLayout->addWidget(disableLidAction);
    daemonContainerLayout->addStretch();

    // screensaver
    const auto ssContainer = new QWidget(this);
    const auto ssContainerLayout = new QHBoxLayout(ssContainer);
    ssContainerLayout->setMargin(0);

    const auto screensaverBlankLabel = new QLabel(tr("Blank screen"), this);
    screensaverBlank = new QSpinBox(this);
    screensaverBlank->setMaximumWidth(MAX_WIDTH);
    screensaverBlank->setMinimumWidth(MAX_WIDTH);
    screensaverBlank->setMinimum(1);
    screensaverBlank->setMaximum(1000);
    screensaverBlank->setSuffix(QString(" %1").arg(tr("min")));
    screensaverBlank->setPrefix(tr("After "));

    ssContainerLayout->addWidget(screensaverBlankLabel);
    ssContainerLayout->addWidget(screensaverBlank);

    const auto screensaverLockLabel = new QLabel(tr("Lock screen"), this);
    screensaverLock = new QSpinBox(this);
    screensaverLock->setMaximumWidth(MAX_WIDTH);
    screensaverLock->setMinimumWidth(MAX_WIDTH);
    screensaverLock->setMinimum(1);
    screensaverLock->setMaximum(1000);
    screensaverLock->setSuffix(QString(" %1").arg(tr("min")));
    screensaverLock->setPrefix(tr("After "));

    ssContainerLayout->addWidget(screensaverLockLabel);
    ssContainerLayout->addWidget(screensaverLock);

    // notify
    const auto notifyContainer = new QGroupBox(this);
    notifyContainer->setTitle(tr("Notifications"));
    const auto notifyContainerLayout = new QVBoxLayout(notifyContainer);

    showNotifications = new QCheckBox(this);
    showNotifications->setText(tr("Show notifications"));
    showNotifications->setToolTip(tr("Show notifications for power related events."));

    warnOnLowBattery = new QCheckBox(this);
    warnOnLowBattery->setText(tr("Notify on low battery"));
    warnOnLowBattery->setToolTip(tr("Show a notification when on low battery (%1% over critical)")
                                 .arg(POWERKIT_LOW_BATTERY));

    warnOnVeryLowBattery = new QCheckBox(this);
    warnOnVeryLowBattery->setText(tr("Notify on very low battery"));
    warnOnVeryLowBattery->setToolTip(tr("Show a notification when on very low battery (1% over critical)"));

    notifyOnBattery = new QCheckBox(this);
    notifyOnBattery->setText(tr("Notify on battery"));
    notifyOnBattery->setToolTip(tr("Notify when switched on battery power"));

    notifyOnAC = new QCheckBox(this);
    notifyOnAC->setText(tr("Notify on AC"));
    notifyOnAC->setToolTip(tr("Notify when switched on AC power"));

    notifyNewInhibitor = new QCheckBox(this);
    notifyNewInhibitor->setText(tr("Notify on new inhibitors"));
    notifyNewInhibitor->setToolTip(tr("Notify on new screensaver or power inhibitors"));

    notifyContainerLayout->addWidget(showNotifications);
    notifyContainerLayout->addWidget(warnOnLowBattery);
    notifyContainerLayout->addWidget(warnOnVeryLowBattery);
    notifyContainerLayout->addWidget(notifyOnBattery);
    notifyContainerLayout->addWidget(notifyOnAC);
    notifyContainerLayout->addWidget(notifyNewInhibitor);
    notifyContainerLayout->addStretch();

    const auto commonWidget = new QWidget(this);
    const auto commonWidgetLayout = new QVBoxLayout(commonWidget);

    const auto optContainer = new QWidget(this);
    const auto optContainerLayout = new QHBoxLayout(optContainer);
    optContainerLayout->setMargin(0);

    const auto batteryAcContainer = new QWidget(this);
    const auto batteryAcContainerLayout = new QHBoxLayout(batteryAcContainer);
    batteryAcContainerLayout->setMargin(0);

    batteryAcContainerLayout->addWidget(batteryContainer);
    batteryAcContainerLayout->addWidget(acContainer);

    optContainerLayout->addWidget(daemonContainer);
    optContainerLayout->addWidget(notifyContainer);

    commonWidgetLayout->addWidget(batteryAcContainer);
    commonWidgetLayout->addWidget(optContainer);
    commonWidgetLayout->addWidget(ssContainer);

    const auto scrollArea = new QScrollArea(this);
    scrollArea->setSizePolicy(QSizePolicy::Expanding,
                              QSizePolicy::Expanding);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: 0; }");
    scrollArea->setWidget(commonWidget);

    layout->addWidget(statusWidget);
    layout->addWidget(scrollArea);

    handleUpdatedDevices();
}

// populate widgets with default values
void Dialog::populateWidgets()
{
    lidActionBattery->clear();
    lidActionBattery->addItem(tr("None"), lidNone);
    lidActionBattery->addItem(tr("Lock Screen"), lidLock);
    lidActionBattery->addItem(tr("Sleep"), lidSleep);
    lidActionBattery->addItem(tr("Hibernate"), lidHibernate);
    lidActionBattery->addItem(tr("Shutdown"), lidShutdown);
    lidActionBattery->addItem(tr("Hybrid Sleep"), lidHybridSleep);
    lidActionBattery->addItem(tr("Sleep then Hibernate"), lidSleepHibernate);

    lidActionAC->clear();
    lidActionAC->addItem(tr("None"), lidNone);
    lidActionAC->addItem(tr("Lock Screen"), lidLock);
    lidActionAC->addItem(tr("Sleep"), lidSleep);
    lidActionAC->addItem(tr("Hibernate"), lidHibernate);
    lidActionAC->addItem(tr("Shutdown"), lidShutdown);
    lidActionAC->addItem(tr("Hybrid Sleep"), lidHybridSleep);
    lidActionAC->addItem(tr("Sleep then Hibernate"), lidSleepHibernate);

    criticalActionBattery->clear();
    criticalActionBattery->addItem(tr("None"), criticalNone);
    criticalActionBattery->addItem(tr("Sleep"), criticalSuspend);
    criticalActionBattery->addItem(tr("Hibernate"), criticalHibernate);
    criticalActionBattery->addItem(tr("Shutdown"), criticalShutdown);

    autoSleepBatteryAction->clear();
    autoSleepBatteryAction->addItem(tr("None"), suspendNone);
    autoSleepBatteryAction->addItem(tr("Sleep"), suspendSleep);
    autoSleepBatteryAction->addItem(tr("Hibernate"), suspendHibernate);
    autoSleepBatteryAction->addItem(tr("Shutdown"), suspendShutdown);
    autoSleepBatteryAction->addItem(tr("Hybrid Sleep"), suspendHybrid);
    autoSleepBatteryAction->addItem(tr("Sleep then Hibernate"), suspendSleepHibernate);

    autoSleepACAction->clear();
    autoSleepACAction->addItem(tr("None"), suspendNone);
    autoSleepACAction->addItem(tr("Sleep"), suspendSleep);
    autoSleepACAction->addItem(tr("Hibernate"), suspendHibernate);
    autoSleepACAction->addItem(tr("Shutdown"), suspendShutdown);
    autoSleepACAction->addItem(tr("Hybrid Sleep"), suspendHybrid);
    autoSleepACAction->addItem(tr("Sleep then Hibernate"), suspendSleepHibernate);
}

void Dialog::connectWidgets()
{
    connect(lidActionBattery, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleLidActionBattery(int)));
    connect(lidActionAC, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleLidActionAC(int)));
    connect(criticalActionBattery, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleCriticalAction(int)));
    connect(criticalBattery, SIGNAL(valueChanged(int)),
            this, SLOT(handleCriticalBattery(int)));
    connect(autoSleepBattery, SIGNAL(valueChanged(int)),
            this, SLOT(handleAutoSleepBattery(int)));
    connect(autoSleepAC, SIGNAL(valueChanged(int)),
            this, SLOT(handleAutoSleepAC(int)));
    connect(showNotifications, SIGNAL(toggled(bool)),
            this, SLOT(handleShowNotifications(bool)));
    connect(showSystemTray, SIGNAL(toggled(bool)),
            this, SLOT(handleShowSystemTray(bool)));
    connect(disableLidAction, SIGNAL(toggled(bool)),
            this, SLOT(handleDisableLidAction(bool)));
    connect(autoSleepBatteryAction, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleAutoSleepBatteryAction(int)));
    connect(autoSleepACAction, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleAutoSleepACAction(int)));
    connect(backlightBatteryCheck, SIGNAL(toggled(bool)),
            this, SLOT(handleBacklightBatteryCheck(bool)));
    connect(backlightACCheck, SIGNAL(toggled(bool)),
            this, SLOT(handleBacklightACCheck(bool)));
    connect(backlightSliderBattery, SIGNAL(valueChanged(int)),
            this, SLOT(handleBacklightBatterySlider(int)));
    connect(backlightSliderAC, SIGNAL(valueChanged(int)),
            this, SLOT(handleBacklightACSlider(int)));
    connect(backlightBatteryLowerCheck, SIGNAL(toggled(bool)),
            this, SLOT(handleBacklightBatteryCheckLower(bool)));
    connect(backlightACHigherCheck, SIGNAL(toggled(bool)),
            this, SLOT(handleBacklightACCheckHigher(bool)));
    connect(warnOnLowBattery, SIGNAL(toggled(bool)),
            this, SLOT(handleWarnOnLowBattery(bool)));
    connect(warnOnVeryLowBattery, SIGNAL(toggled(bool)),
            this, SLOT(handleWarnOnVeryLowBattery(bool)));
    connect(notifyOnBattery, SIGNAL(toggled(bool)),
            this, SLOT(handleNotifyBattery(bool)));
    connect(notifyOnAC, SIGNAL(toggled(bool)),
            this, SLOT(handleNotifyAC(bool)));
    connect(notifyNewInhibitor, SIGNAL(toggled(bool)),
            this, SLOT(handleNotifyNewInhibitor(bool)));
    connect(backlightMouseWheel, SIGNAL(toggled(bool)),
            this, SLOT(handleBacklightMouseWheel(bool)));
    connect(screensaverBlank, SIGNAL(valueChanged(int)),
            this, SLOT(handleScreensaverBlank(int)));
    connect(screensaverLock, SIGNAL(valueChanged(int)),
            this, SLOT(handleScreensaverLock(int)));
}

// load settings and set defaults
void Dialog::loadSettings()
{
    if (Settings::isValid(CONF_DIALOG_GEOMETRY)) {
        restoreGeometry(Settings::getValue(CONF_DIALOG_GEOMETRY).toByteArray());
    }

    int defaultAutoSleepBattery = POWERKIT_AUTO_SLEEP_BATTERY;
    if (Settings::isValid(CONF_SUSPEND_BATTERY_TIMEOUT)) {
        defaultAutoSleepBattery = Settings::getValue(CONF_SUSPEND_BATTERY_TIMEOUT).toInt();
    }
    setDefaultAction(autoSleepBattery, defaultAutoSleepBattery);

    int defaultAutoSleepBatteryAction = POWERKIT_SUSPEND_BATTERY_ACTION;
    if (Settings::isValid(CONF_SUSPEND_BATTERY_ACTION)) {
        defaultAutoSleepBatteryAction = Settings::getValue(CONF_SUSPEND_BATTERY_ACTION).toInt();
    }
    setDefaultAction(autoSleepBatteryAction, defaultAutoSleepBatteryAction);

    int defaultAutoSleepAC = 0;
    if (Settings::isValid(CONF_SUSPEND_AC_TIMEOUT)) {
        defaultAutoSleepAC = Settings::getValue(CONF_SUSPEND_AC_TIMEOUT).toInt();
    }
    setDefaultAction(autoSleepAC, defaultAutoSleepAC);

    int defaultAutoSleepACAction = POWERKIT_SUSPEND_AC_ACTION;
    if (Settings::isValid(CONF_SUSPEND_AC_ACTION)) {
        defaultAutoSleepACAction = Settings::getValue(CONF_SUSPEND_AC_ACTION).toInt();
    }
    setDefaultAction(autoSleepACAction, defaultAutoSleepACAction);

    int defaultCriticalBattery = POWERKIT_CRITICAL_BATTERY;
    if (Settings::isValid(CONF_CRITICAL_BATTERY_TIMEOUT)) {
        defaultCriticalBattery = Settings::getValue(CONF_CRITICAL_BATTERY_TIMEOUT).toInt();
    }
    setDefaultAction(criticalBattery, defaultCriticalBattery);

    int defaultLidActionBattery = POWERKIT_LID_BATTERY_ACTION;
    if (Settings::isValid(CONF_LID_BATTERY_ACTION)) {
        defaultLidActionBattery = Settings::getValue(CONF_LID_BATTERY_ACTION).toInt();
    }
    setDefaultAction(lidActionBattery, defaultLidActionBattery);

    int defaultLidActionAC = POWERKIT_LID_AC_ACTION;
    if (Settings::isValid(CONF_LID_AC_ACTION)) {
        defaultLidActionAC = Settings::getValue(CONF_LID_AC_ACTION).toInt();
    }
    setDefaultAction(lidActionAC, defaultLidActionAC);

    int defaultCriticalAction = POWERKIT_CRITICAL_ACTION;
    if (Settings::isValid(CONF_CRITICAL_BATTERY_ACTION)) {
        defaultCriticalAction = Settings::getValue(CONF_CRITICAL_BATTERY_ACTION).toInt();
    }
    setDefaultAction(criticalActionBattery, defaultCriticalAction);

    bool defaultShowNotifications = true;
    if (Settings::isValid(CONF_TRAY_NOTIFY)) {
        defaultShowNotifications = Settings::getValue(CONF_TRAY_NOTIFY).toBool();
    }
    showNotifications->setChecked(defaultShowNotifications);

    bool defaultShowTray = true;
    if (Settings::isValid(CONF_TRAY_SHOW)) {
        defaultShowTray = Settings::getValue(CONF_TRAY_SHOW).toBool();
    }
    showSystemTray->setChecked(defaultShowTray);

    bool defaultDisableLidAction = true;
    if (Settings::isValid(CONF_LID_DISABLE_IF_EXTERNAL)) {
        defaultDisableLidAction = Settings::getValue(CONF_LID_DISABLE_IF_EXTERNAL).toBool();
    }
    disableLidAction->setChecked(defaultDisableLidAction);

    bool defaultWarnOnLowBattery = true;
    if (Settings::isValid(CONF_WARN_ON_LOW_BATTERY)) {
        defaultWarnOnLowBattery = Settings::getValue(CONF_WARN_ON_LOW_BATTERY).toBool();
    }
    warnOnLowBattery->setChecked(defaultWarnOnLowBattery);

    bool defaultWarnOnVeryLowBattery = true;
    if (Settings::isValid(CONF_WARN_ON_VERYLOW_BATTERY)) {
        defaultWarnOnVeryLowBattery = Settings::getValue(CONF_WARN_ON_VERYLOW_BATTERY).toBool();
    }
    warnOnVeryLowBattery->setChecked(defaultWarnOnVeryLowBattery);

    bool defaultNotifyOnBattery = true;
    if (Settings::isValid(CONF_NOTIFY_ON_BATTERY)) {
        defaultNotifyOnBattery = Settings::getValue(CONF_NOTIFY_ON_BATTERY).toBool();
    }
    notifyOnBattery->setChecked(defaultNotifyOnBattery);

    bool defaultNotifyOnAC = true;
    if (Settings::isValid(CONF_NOTIFY_ON_AC)) {
        defaultNotifyOnAC = Settings::getValue(CONF_NOTIFY_ON_AC).toBool();
    }
    notifyOnAC->setChecked(defaultNotifyOnAC);

    bool defaultNotifyNewInhibitor = true;
    if (Settings::isValid(CONF_NOTIFY_NEW_INHIBITOR)) {
        defaultNotifyNewInhibitor = Settings::getValue(CONF_NOTIFY_NEW_INHIBITOR).toBool();
    }
    notifyNewInhibitor->setChecked(defaultNotifyNewInhibitor);

    // check
    checkPerms();

    // backlight
    backlightDevice = Backlight::getDevice();

    backlightSliderAC->setEnabled(true);
    backlightSliderBattery->setEnabled(true);

    int backlightMin = 1;
    int backlightMax = Backlight::getMaxBrightness(backlightDevice);

    backlightSliderBattery->setMinimum(backlightMin);
    backlightSliderBattery->setMaximum(backlightMax);
    backlightSliderBattery->setValue(backlightSliderBattery->maximum());
    backlightSliderAC->setMinimum(backlightMin);
    backlightSliderAC->setMaximum(backlightMax);
    backlightSliderAC->setValue(backlightSliderAC->maximum());

    backlightBatteryCheck->setChecked(Settings::getValue(CONF_BACKLIGHT_BATTERY_ENABLE)
                                      .toBool());
    backlightACCheck->setChecked(Settings::getValue(CONF_BACKLIGHT_AC_ENABLE)
                                 .toBool());
    if (Settings::isValid(CONF_BACKLIGHT_BATTERY)) {
        backlightSliderBattery->setValue(Settings::getValue(CONF_BACKLIGHT_BATTERY)
                                         .toInt());
    }
    if (Settings::isValid(CONF_BACKLIGHT_AC)) {
        backlightSliderAC->setValue(Settings::getValue(CONF_BACKLIGHT_AC)
                                    .toInt());
    }
    if (Settings::isValid(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER)) {
        backlightBatteryLowerCheck->setChecked(
                    Settings::getValue(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER)
                    .toBool());
    }
    if (Settings::isValid(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER)) {
        backlightACHigherCheck->setChecked(
                    Settings::getValue(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER)
                    .toBool());
    }
    bool defaultBacklightMouseWheel = true;
    if (Settings::isValid(CONF_BACKLIGHT_MOUSE_WHEEL)) {
        defaultBacklightMouseWheel = Settings::getValue(CONF_BACKLIGHT_MOUSE_WHEEL).toBool();
    }
    backlightMouseWheel->setChecked(defaultBacklightMouseWheel);

    enableBacklight(true);
    enableLid(Client::lidIsPresent(dbus));
    hasCpuCoreTemp = Cpu::hasCoreTemp();
    if (!hasCpuCoreTemp) { cpuTempLabel->setVisible(false); }

    screensaverBlank->setValue(Settings::getValue(CONF_SCREENSAVER_TIMEOUT_BLANK,
                                                  POWERKIT_SCREENSAVER_TIMEOUT_BLANK).toInt() / 60);

    screensaverLock->setValue(Settings::getValue(CONF_SCREENSAVER_TIMEOUT_LOCK,
                                                  POWERKIT_SCREENSAVER_TIMEOUT_LOCK).toInt() / 60);
}

void Dialog::saveSettings()
{
    Settings::setValue(CONF_LID_BATTERY_ACTION,
                       lidActionBattery->currentData().toInt());
    Settings::setValue(CONF_LID_AC_ACTION,
                       lidActionAC->currentData().toInt());
    Settings::setValue(CONF_CRITICAL_BATTERY_ACTION,
                       criticalActionBattery->currentData().toInt());
    Settings::setValue(CONF_CRITICAL_BATTERY_TIMEOUT,
                       criticalBattery->value());
    Settings::setValue(CONF_SUSPEND_BATTERY_TIMEOUT,
                       autoSleepBattery->value());
    Settings::setValue(CONF_SUSPEND_AC_TIMEOUT,
                       autoSleepAC->value());
    Settings::setValue(CONF_TRAY_NOTIFY,
                       showNotifications->isChecked());
    Settings::setValue(CONF_TRAY_SHOW,
                       showSystemTray->isChecked());
    Settings::setValue(CONF_LID_DISABLE_IF_EXTERNAL,
                       disableLidAction->isChecked());
    Settings::setValue(CONF_SUSPEND_BATTERY_ACTION,
                       autoSleepBatteryAction->currentData().toInt());
    Settings::setValue(CONF_SUSPEND_AC_ACTION,
                       autoSleepACAction->currentData().toInt());
    Settings::setValue(CONF_BACKLIGHT_BATTERY_ENABLE,
                       backlightBatteryCheck->isChecked());
    Settings::setValue(CONF_BACKLIGHT_AC_ENABLE,
                       backlightACCheck->isChecked());
    Settings::setValue(CONF_BACKLIGHT_BATTERY,
                       backlightSliderBattery->value());
    Settings::setValue(CONF_BACKLIGHT_AC,
                       backlightSliderAC->value());
    Settings::setValue(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER,
                       backlightBatteryLowerCheck->isChecked());
    Settings::setValue(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER,
                       backlightACHigherCheck->isChecked());
    Settings::setValue(CONF_DIALOG,
                       saveGeometry());
    Settings::setValue(CONF_WARN_ON_LOW_BATTERY,
                       warnOnLowBattery->isChecked());
    Settings::setValue(CONF_WARN_ON_VERYLOW_BATTERY,
                       warnOnVeryLowBattery->isChecked());
    Settings::setValue(CONF_NOTIFY_ON_BATTERY,
                       notifyOnBattery->isChecked());
    Settings::setValue(CONF_NOTIFY_ON_AC,
                       notifyOnAC->isChecked());
    Settings::setValue(CONF_NOTIFY_NEW_INHIBITOR,
                       notifyNewInhibitor->isChecked());
    Settings::setValue(CONF_BACKLIGHT_MOUSE_WHEEL,
                       backlightMouseWheel->isChecked());
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

// set default value in combobox
void Dialog::setDefaultAction(QComboBox *box, QString value)
{
    for (int i=0;i<box->count();i++) {
        if (box->itemText(i) == value) {
            box->setCurrentIndex(i);
            return;
        }
    }
}

void Dialog::drawBattery()
{
    if (!hasBattery) { return; }
    const auto battery = Client::getBatteryLeft(dbus);
    const auto onBattery = Client::onBattery(dbus);
    QString batteryTime = QDateTime::fromTime_t(onBattery ? Client::timeToEmpty(dbus) : Client::timeToFull(dbus)).toUTC().toString("hh:mm");
    QColor colorBg = Qt::green;
    QColor colorFg = Qt::white;
    QString status = QString("%1%\n%2").arg(QString::number(battery), batteryTime);
    if (onBattery) {
        if (battery >= 26) {
            colorBg = QColor("orange");
        } else {
            colorBg = Qt::red;
        }
    } else {
        if (battery == 100) {
            colorFg = Qt::green;
            status = QString("%1%").arg(QString::number(battery));
        }
    }
    batteryStatusLabel->setPixmap(Theme::drawCircleProgress(battery,
                                                            64,
                                                            4,
                                                            4,
                                                            true,
                                                            status,
                                                            colorBg,
                                                            colorFg));
}

void Dialog::drawCpu()
{
    if (cpuFreqLabel) {
        const auto freq = Cpu::getCpuFreqLabel();
        QColor color = Qt::gray;
        if (freq.first >= 50) { color = QColor("orange"); }
        cpuFreqLabel->setPixmap(Theme::drawCircleProgress(freq.first,
                                                          64,
                                                          4,
                                                          4,
                                                          true,
                                                          freq.second,
                                                          color,
                                                          Qt::white));
    }
    if (cpuTempLabel && hasCpuCoreTemp) {
        const auto temp = Cpu::getCpuTempLabel();
        QColor color = Qt::gray;
        if (temp.first >= 75) { color = Qt::red; }
        else if (temp.first >= 50) { color = QColor("orange"); }
        cpuTempLabel->setPixmap(Theme::drawCircleProgress(temp.first,
                                                          64,
                                                          4,
                                                          4,
                                                          true,
                                                          temp.second,
                                                          color,
                                                          Qt::white));
    }
}

void Dialog::handleUpdatedDevices()
{
    hasBattery = Client::hasBattery(dbus);
    batteryStatusLabel->setVisible(hasBattery);
    drawBattery();
    drawCpu();
}

// save current value and update power manager
void Dialog::handleLidActionBattery(int index)
{
    checkPerms();
    Settings::setValue(CONF_LID_BATTERY_ACTION,
                       lidActionBattery->itemData(index).toInt());
}

void Dialog::handleLidActionAC(int index)
{
    checkPerms();
    Settings::setValue(CONF_LID_AC_ACTION,
                       lidActionAC->itemData(index).toInt());
}

void Dialog::handleCriticalAction(int index)
{
    checkPerms();
    Settings::setValue(CONF_CRITICAL_BATTERY_ACTION,
                       criticalActionBattery->itemData(index).toInt());
}

void Dialog::handleCriticalBattery(int value)
{
    Settings::setValue(CONF_CRITICAL_BATTERY_TIMEOUT, value);
}

void Dialog::handleAutoSleepBattery(int value)
{
    Settings::setValue(CONF_SUSPEND_BATTERY_TIMEOUT, value);
 }

void Dialog::handleAutoSleepAC(int value)
{
    Settings::setValue(CONF_SUSPEND_AC_TIMEOUT, value);
}

void Dialog::handleShowNotifications(bool triggered)
{
    Settings::setValue(CONF_TRAY_NOTIFY, triggered);
}

void Dialog::handleShowSystemTray(bool triggered)
{
    Settings::setValue(CONF_TRAY_SHOW, triggered);
}

void Dialog::handleDisableLidAction(bool triggered)
{
    Settings::setValue(CONF_LID_DISABLE_IF_EXTERNAL, triggered);
}

void Dialog::handleAutoSleepBatteryAction(int index)
{
    checkPerms();
    Settings::setValue(CONF_SUSPEND_BATTERY_ACTION,
                       autoSleepBatteryAction->itemData(index).toInt());
}

void Dialog::handleAutoSleepACAction(int index)
{
    checkPerms();
    Settings::setValue(CONF_SUSPEND_AC_ACTION,
                       autoSleepACAction->itemData(index).toInt());
}

void Dialog::checkPerms()
{
    if (!Client::canHibernate(dbus)) {
        bool warnCantHibernate = false;
        if (criticalActionBattery->currentData().toInt() == criticalHibernate) {
            warnCantHibernate = true;
            criticalActionBattery->setCurrentIndex(criticalShutdown);
            handleCriticalAction(criticalShutdown);
        }
        if (lidActionAC->currentData().toInt() == lidHibernate ||
            lidActionAC->currentData().toInt() == lidHybridSleep) {
            warnCantHibernate = true;
            lidActionAC->setCurrentIndex(lidSleep);
            handleLidActionAC(lidSleep);
        }
        if (lidActionBattery->currentData().toInt() == lidHibernate ||
            lidActionBattery->currentData().toInt() == lidHybridSleep) {
            warnCantHibernate = true;
            lidActionBattery->setCurrentIndex(lidSleep);
            handleLidActionBattery(lidSleep);
        }
        if (autoSleepACAction->currentData().toInt() == suspendHibernate ||
            autoSleepACAction->currentData().toInt() == suspendHybrid) {
            warnCantHibernate = true;
            autoSleepACAction->setCurrentIndex(suspendSleep);
            handleAutoSleepACAction(suspendSleep);
        }
        if (autoSleepBatteryAction->currentData().toInt() == suspendHibernate ||
            autoSleepBatteryAction->currentData().toInt() == suspendHybrid) {
            warnCantHibernate = true;
            autoSleepBatteryAction->setCurrentIndex(suspendSleep);
            handleAutoSleepBatteryAction(suspendSleep);
        }
        if (warnCantHibernate) { hibernateWarn(); }
    }
    if (!Client::canSuspend(dbus)) {
        bool warnCantSleep = false;
        if (lidActionAC->currentData().toInt() == lidSleep) {
            warnCantSleep = true;
            lidActionAC->setCurrentIndex(lidLock);
            handleLidActionAC(lidLock);
        }
        if (lidActionBattery->currentData().toInt() == lidSleep) {
            warnCantSleep = true;
            lidActionBattery->setCurrentIndex(lidLock);
            handleLidActionBattery(lidLock);
        }
        if (autoSleepACAction->currentData().toInt() == suspendSleep) {
            warnCantSleep = true;
            autoSleepACAction->setCurrentIndex(suspendNone);
            handleAutoSleepACAction(suspendNone);
        }
        if (autoSleepBatteryAction->currentData().toInt() == suspendSleep) {
            warnCantSleep = true;
            autoSleepBatteryAction->setCurrentIndex(suspendNone);
            handleAutoSleepBatteryAction(suspendNone);
        }
        if (warnCantSleep) { sleepWarn(); }
    }
}

void Dialog::handleBacklightBatteryCheck(bool triggered)
{
    Settings::setValue(CONF_BACKLIGHT_BATTERY_ENABLE, triggered);
}

void Dialog::handleBacklightACCheck(bool triggered)
{
    Settings::setValue(CONF_BACKLIGHT_AC_ENABLE, triggered);
}

void Dialog::handleBacklightBatterySlider(int value)
{
    Settings::setValue(CONF_BACKLIGHT_BATTERY, value);
}

void Dialog::handleBacklightACSlider(int value)
{
    Settings::setValue(CONF_BACKLIGHT_AC, value);
}

void Dialog::hibernateWarn()
{
    QMessageBox::warning(this, tr("Hibernate not supported"),
                         tr("Hibernate not supported, consult your OS documentation."));
}

void Dialog::sleepWarn()
{
    QMessageBox::warning(this, tr("Sleep not supported"),
                         tr("Sleep not supported, consult your OS documentation."));
}

void Dialog::handleBacklightBatteryCheckLower(bool triggered)
{
    Settings::setValue(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER, triggered);
}

void Dialog::handleBacklightACCheckHigher(bool triggered)
{
    Settings::setValue(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER, triggered);
}

void Dialog::enableBacklight(bool enabled)
{
    backlightSliderBattery->setEnabled(enabled);
    backlightSliderAC->setEnabled(enabled);
    backlightBatteryCheck->setEnabled(enabled);
    backlightACCheck->setEnabled(enabled);
    backlightBatteryLowerCheck->setEnabled(enabled);
    backlightACHigherCheck->setEnabled(enabled);
    batteryBacklightLabel->setEnabled(enabled);
    acBacklightLabel->setEnabled(enabled);
    backlightMouseWheel->setEnabled(enabled);
}

void Dialog::handleWarnOnLowBattery(bool triggered)
{
    Settings::setValue(CONF_WARN_ON_LOW_BATTERY, triggered);
}

void Dialog::handleWarnOnVeryLowBattery(bool triggered)
{
    Settings::setValue(CONF_WARN_ON_VERYLOW_BATTERY, triggered);
}

void Dialog::handleNotifyBattery(bool triggered)
{
    Settings::setValue(CONF_NOTIFY_ON_BATTERY, triggered);
}

void Dialog::handleNotifyAC(bool triggered)
{
    Settings::setValue(CONF_NOTIFY_ON_AC, triggered);
}

void Dialog::handleNotifyNewInhibitor(bool triggered)
{
    Settings::setValue(CONF_NOTIFY_NEW_INHIBITOR, triggered);
}

void Dialog::enableLid(bool enabled)
{
    lidActionAC->setEnabled(enabled);
    lidActionBattery->setEnabled(enabled);
    lidActionACLabel->setEnabled(enabled);
    lidActionBatteryLabel->setEnabled(enabled);
    disableLidAction->setEnabled(enabled);
}

void Dialog::handleBacklightMouseWheel(bool triggered)
{
    Settings::setValue(CONF_BACKLIGHT_MOUSE_WHEEL, triggered);
}

void Dialog::handleScreensaverBlank(int value)
{
    Settings::setValue(CONF_SCREENSAVER_TIMEOUT_BLANK, value * 60);
}

void Dialog::handleScreensaverLock(int value)
{
    Settings::setValue(CONF_SCREENSAVER_TIMEOUT_LOCK, value * 60);
}
