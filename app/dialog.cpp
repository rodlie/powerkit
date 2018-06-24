/*
# PowerDwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#
*/

#include "dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , dbus(0)
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
    , autoSleepACAction(0)
    , lockscreenButton(0)
    , sleepButton(0)
    , hibernateButton(0)
    , poweroffButton(0)
    , lowBatteryAction(0)
    , monitorList(0)
    , monitorModes(0)
    , monitorRates(0)
    , monitorPrimary(0)
    , monitorSaveButton(0)
    , monitorApplyButton(0)
    , monitorRotation(0)
    , monitorPosition(0)
    , monitorPositionOther(0)
{
    // setup dialog
    setAttribute(Qt::WA_QuitOnClose, true);
    setWindowTitle(tr("Power Manager"));
    setWindowIcon(QIcon::fromTheme(DEFAULT_BATTERY_ICON, QIcon(":/icons/battery.png")));
    setMinimumSize(QSize(490,390));

    // setup dbus
    QDBusConnection session = QDBusConnection::sessionBus();
    dbus = new QDBusInterface(PD_SERVICE, PD_PATH, PD_SERVICE, session, this);
    if (dbus->isValid()) {
        session.connect(dbus->service(), dbus->path(), dbus->service(), "updatedMonitors", this, SLOT(handleUpdatedMonitors()));
    } else {
        QMessageBox::warning(this, tr("Power manager not running"), tr("Power manager is not running, please start it before running settings."));
        QTimer::singleShot(100, this, SLOT(close()));
    }

    // setup theme
    Common::setIconTheme();

    // setup widgets
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(5);
    layout->setSpacing(0);

    QTabWidget *containerWidget = new QTabWidget(this);

    QWidget *wrapper = new QWidget(this);
    wrapper->setContentsMargins(0,0,0,0);

    QVBoxLayout *wrapperLayout = new QVBoxLayout(wrapper);
    wrapperLayout->setMargin(0);
    wrapperLayout->setSpacing(0);

    //QWidget *powerContainer = new QWidget(this);
    //powerContainer->setContentsMargins(0,0,0,0);

    /*if (!Common::vendor().isEmpty()) {
        containerWidget->setTabPosition(QTabWidget::South);
        QHBoxLayout *powerContainerLayout = new QHBoxLayout(powerContainer);
        powerContainerLayout->setMargin(0);
        powerContainerLayout->setSpacing(0);

        QLabel *powerLabel = new QLabel(this);
        QIcon powerIcon = QIcon(QString(":/icons/vendors/%1-black.png").arg(Common::vendor()));
        powerLabel->setPixmap(powerIcon.pixmap(QSize(350, 75)));
        powerLabel->setMinimumSize(350, 75);
        powerLabel->setMaximumSize(powerLabel->minimumSize());

        QLabel *powerBatteryLabel = new QLabel(this);
        powerBatteryLabel->setText(tr("<h1 style=\"font-weight:normal;\">Power Manager</h1>PowerDwarf version %1.<br><a href=\"https://github.com/rodlie/powerdwarf\">https://github.com/rodlie/powerdwarf</a>").arg(QApplication::applicationVersion()));

        powerContainerLayout->addWidget(powerLabel);
        powerContainerLayout->addStretch();
        powerContainerLayout->addWidget(powerBatteryLabel);
        wrapperLayout->addWidget(powerContainer);
    }*/
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

    QWidget *lowActionBatteryContainer = new QWidget(this);
    lowActionBatteryContainer->setContentsMargins(0,0,0,0);
    QVBoxLayout *lowActionBatteryContainerLayout = new QVBoxLayout(lowActionBatteryContainer);
    lowActionBatteryContainerLayout->setMargin(0);
    lowActionBatteryContainerLayout->setSpacing(0);
    lowBatteryAction = new QComboBox(this);
    connect(lowBatteryAction, SIGNAL(currentIndexChanged(int)), this, SLOT(handleLowBatteryAction(int)));
    lowActionBatteryContainerLayout->addWidget(lowBattery);
    lowActionBatteryContainerLayout->addWidget(lowBatteryAction);

    QLabel *lowBatteryIcon = new QLabel(this);
    lowBatteryIcon->setMaximumSize(48,48);
    lowBatteryIcon->setMinimumSize(48,48);
    lowBatteryIcon->setPixmap(QIcon::fromTheme("battery-low-charging", QIcon(":/icons/battery-low-charging.png")).pixmap(QSize(48,48)));
    lowBatteryLabel->setText(tr("<h2 style=\"font-weight:normal;\">Low battery</h2>"));
    lowBatteryContainerLayout->addWidget(lowBatteryIcon);
    lowBatteryContainerLayout->addWidget(lowBatteryLabel);
    lowBatteryContainerLayout->addWidget(lowActionBatteryContainer);
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
    criticalBatteryIcon->setMaximumSize(48,48);
    criticalBatteryIcon->setMinimumSize(48,48);
    criticalBatteryIcon->setPixmap(QIcon::fromTheme("battery-caution", QIcon(":/icons/battery-caution.png")).pixmap(QSize(48,48)));
    criticalBatteryLabel->setText(tr("<h2 style=\"font-weight:normal;\">Critical battery</h2>"));
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
    sleepBatteryIcon->setMaximumSize(48,48);
    sleepBatteryIcon->setMinimumSize(48,48);
    sleepBatteryIcon->setPixmap(QIcon::fromTheme("system-suspend", QIcon(":/icons/system-suspend.png")).pixmap(QSize(48,48)));
    sleepBatteryLabel->setText(tr("<h2 style=\"font-weight:normal;\">Suspend after</h2>"));
    sleepBatteryContainerLayout->addWidget(sleepBatteryIcon);
    sleepBatteryContainerLayout->addWidget(sleepBatteryLabel);
    sleepBatteryContainerLayout->addWidget(sleepActionBatteryContainer);
    batteryContainerLayout->addWidget(sleepBatteryContainer);

    batteryContainerLayout->addStretch();

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
    //sleepACContainerLayout->addWidget(autoSleepAC);
    acContainerLayout->addWidget(sleepACContainer);


    QWidget *sleepActionACContainer = new QWidget(this);
    sleepActionACContainer->setContentsMargins(0,0,0,0);
    QVBoxLayout *sleepActionACContainerLayout = new QVBoxLayout(sleepActionACContainer);
    sleepActionACContainerLayout->setMargin(0);
    sleepActionACContainerLayout->setSpacing(0);
    autoSleepACAction = new QComboBox(this);
    sleepActionACContainerLayout->addWidget(autoSleepAC);
    sleepActionACContainerLayout->addWidget(autoSleepACAction);
    //acContainerLayout->addWidget(sleepActionACContainer);

    QLabel *sleepACIcon = new QLabel(this);
    sleepACIcon->setMaximumSize(48,48);
    sleepACIcon->setMinimumSize(48,48);
    sleepACIcon->setPixmap(QIcon::fromTheme("system-suspend", QIcon(":/icons/system-suspend.png")).pixmap(QSize(48,48)));
    sleepACLabel->setText(tr("<h2 style=\"font-weight:normal;\">Suspend after</h2>"));
    sleepACContainerLayout->addWidget(sleepACIcon);
    sleepACContainerLayout->addWidget(sleepACLabel);
    sleepACContainerLayout->addWidget(sleepActionACContainer);
    acContainerLayout->addWidget(sleepACContainer);




    acContainerLayout->addStretch();

    QWidget *advContainer = new QWidget(this);
    QVBoxLayout *advContainerLayout = new QVBoxLayout(advContainer);

    showSystemTray  = new QCheckBox(this);
    showSystemTray->setIcon(QIcon::fromTheme("preferences-other", QIcon(":/icons/preferences-other.png")));
    showSystemTray->setText(tr("Show system tray"));

    showNotifications = new QCheckBox(this);
    showNotifications->setIcon(QIcon::fromTheme("user-available", QIcon(":/icons/user-available.png")));
    showNotifications->setText(tr("Show system tray notifications"));

    showBatteryPercent = new QCheckBox(this);
    showBatteryPercent->setIcon(QIcon::fromTheme("battery", QIcon(":/icons/battery.png")));
    showBatteryPercent->setText(tr("Show battery percent in system tray."));

    desktopSS = new QCheckBox(this);
    desktopSS->setIcon(QIcon::fromTheme("video-display", QIcon(":/icons/video-display.png")));
    desktopSS->setText("org.freedesktop.ScreenSaver");

    desktopPM = new QCheckBox(this);
    desktopPM->setIcon(QIcon::fromTheme("battery", QIcon(":/icons/battery.png")));
    desktopPM->setText("org.freedesktop.PowerManagement");

    advContainerLayout->addWidget(showSystemTray);
    advContainerLayout->addWidget(showNotifications);
    advContainerLayout->addWidget(showBatteryPercent);
    advContainerLayout->addWidget(desktopSS);
    advContainerLayout->addWidget(desktopPM);
    advContainerLayout->addStretch();

    QWidget *extraContainer = new QWidget(this);
    QHBoxLayout *extraContainerLayout = new QHBoxLayout(extraContainer);

    QLabel *powerLabel = new QLabel(this);
    powerLabel->setText(tr("powerdwarf<br>version %1.").arg(QApplication::applicationVersion()));

    lockscreenButton = new QPushButton(this);
    lockscreenButton->setIcon(QIcon::fromTheme("system-lock-screen",
                                               QIcon(":/icons/system-lock-screen.png")));
    lockscreenButton->setIconSize(QSize(24,24));
    lockscreenButton->setToolTip(tr("Lock the screen now."));
    if (lockscreenButton->icon().isNull()) {
        lockscreenButton->setText(tr("Lock screen"));
    }

    sleepButton = new QPushButton(this);
    sleepButton->setIcon(QIcon::fromTheme("system-suspend",
                                          QIcon(":/icons/system-suspend.png")));
    sleepButton->setIconSize(QSize(24,24));
    sleepButton->setToolTip(tr("Suspend computer now."));
    if (sleepButton->icon().isNull()) {
        sleepButton->setText(tr("Suspend"));
    }

    hibernateButton = new QPushButton(this);
    hibernateButton->setIcon(QIcon::fromTheme("system-hibernate",
                                              QIcon(":/icons/system-hibernate.png")));
    hibernateButton->setIconSize(QSize(24,24));
    hibernateButton->setToolTip(tr("Hibernate computer now."));
    if (hibernateButton->icon().isNull()) {
        hibernateButton->setText(tr("Hibernate"));
    }

    poweroffButton = new QPushButton(this);
    poweroffButton->setIcon(QIcon::fromTheme("system-shutdown",
                                             QIcon(":/icons/system-shutdown.png")));
    poweroffButton->setIconSize(QSize(24,24));
    poweroffButton->setToolTip(tr("Shutdown computer now."));
    if (poweroffButton->icon().isNull()) {
        poweroffButton->setText(tr("Shutdown"));
    }

    extraContainerLayout->addWidget(powerLabel);
    extraContainerLayout->addStretch();
    extraContainerLayout->addWidget(lockscreenButton);
    extraContainerLayout->addWidget(sleepButton);
    extraContainerLayout->addWidget(hibernateButton);
    extraContainerLayout->addWidget(poweroffButton);

    QWidget *monitorContainer = new QWidget(this);
    QVBoxLayout *monitorContainerLayout = new QVBoxLayout(monitorContainer);

    monitorList = new QListWidget(this);
    monitorList->setIconSize(QSize(24,24));

    QWidget *monitorModesContainer = new QWidget(this);
    QHBoxLayout *monitorModesContainerLayout = new QHBoxLayout(monitorModesContainer);

    QLabel *monitorModesLabel = new QLabel(this);
    monitorModesLabel->setText(tr("Screen resolution"));
    monitorModes = new QComboBox(this);

    monitorModesContainerLayout->addWidget(monitorModesLabel);
    monitorModesContainerLayout->addWidget(monitorModes);

    QWidget *monitorRatesContainer = new QWidget(this);
    QHBoxLayout *monitorRatesContainerLayout = new QHBoxLayout(monitorRatesContainer);

    QLabel *monitorRatesLabel = new QLabel(this);
    monitorRatesLabel->setText(tr("Screen refresh rate"));
    monitorRates = new QComboBox(this);

    monitorRatesContainerLayout->addWidget(monitorRatesLabel);
    monitorRatesContainerLayout->addWidget(monitorRates);

    QWidget *monitorRotateContainer = new QWidget(this);
    QHBoxLayout *monitorRotateContainerLayout = new QHBoxLayout(monitorRotateContainer);

    QLabel *monitorRotateLabel = new QLabel(this);
    monitorRotateLabel->setText(tr("Screen rotation"));
    monitorRotation = new QComboBox(this);

    monitorRotateContainerLayout->addWidget(monitorRotateLabel);
    monitorRotateContainerLayout->addWidget(monitorRotation);

    QWidget *monitorPosContainer = new QWidget(this);
    QHBoxLayout *monitorPosContainerLayout = new QHBoxLayout(monitorPosContainer);

    QLabel *monitorPosLabel = new QLabel(this);
    monitorPosLabel->setText(tr("Screen position"));
    monitorPosition = new QComboBox(this);
    monitorPositionOther = new QComboBox(this);

    monitorPosContainerLayout->addWidget(monitorPosLabel);
    monitorPosContainerLayout->addWidget(monitorPosition);
    monitorPosContainerLayout->addWidget(monitorPositionOther);

    monitorPrimary = new QCheckBox(this);
    monitorPrimary->setText(tr("Primary screen"));

    QWidget *monitorButtonsContainer = new QWidget(this);
    monitorButtonsContainer->setContentsMargins(0,0,0,0);

    QHBoxLayout *monitorButtonsContainerLayout = new QHBoxLayout(monitorButtonsContainer);
    monitorButtonsContainerLayout->setMargin(0);
    monitorButtonsContainerLayout->addStretch();

    monitorApplyButton = new QPushButton(this);
    monitorApplyButton->setText(tr("Apply monitor changes"));

    monitorSaveButton = new QPushButton(this);
    monitorSaveButton->setText(tr("Save monitor settings"));

    monitorButtonsContainerLayout->addWidget(monitorApplyButton);
    monitorButtonsContainerLayout->addWidget(monitorSaveButton);

    monitorContainerLayout->addWidget(monitorList);
    monitorContainerLayout->addWidget(monitorModesContainer);
    monitorContainerLayout->addWidget(monitorRatesContainer);
    monitorContainerLayout->addWidget(monitorRotateContainer);
    monitorContainerLayout->addWidget(monitorPosContainer);
    monitorContainerLayout->addWidget(monitorPrimary);
    monitorContainerLayout->addWidget(monitorButtonsContainer);

    layout->addWidget(wrapper);
    layout->addWidget(extraContainer);

    containerWidget->addTab(batteryContainer, QIcon::fromTheme("battery", QIcon(":/icons/battery.png")), tr("Battery"));
    containerWidget->addTab(acContainer, QIcon::fromTheme("ac-adapter", QIcon(":/icons/ac-adapter.png")), tr("AC"));
    containerWidget->addTab(monitorContainer, QIcon::fromTheme("video-display", QIcon(":/icons/video-display.png")), tr("Monitors"));
    containerWidget->addTab(advContainer, QIcon::fromTheme("preferences-other", QIcon(":/icons/preferences-other.png")), tr("Advanced"));

    populate(); // populate boxes
    loadSettings(); // load settings

    // connect various widgets
    connect(monitorSaveButton, SIGNAL(released()), this, SLOT(monitorSaveSettings()));
    connect(monitorApplyButton, SIGNAL(released()), this, SLOT(monitorApplySettings()));
    connect(monitorModes, SIGNAL(currentIndexChanged(QString)), this, SLOT(handleMonitorModeChanged(QString)));
    connect(monitorList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(handleMonitorListItemChanged(QListWidgetItem*)));
    connect(monitorList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(handleMonitorListICurrentitemChanged(QListWidgetItem*,QListWidgetItem*)));
    //connect(monitorList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(handleMonitorListItemChanged(QListWidgetItem*)));
    connect(monitorList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(handleMonitorListItemChanged(QListWidgetItem*)));
    connect(monitorList, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(handleMonitorListItemChanged(QListWidgetItem*)));
    connect(lockscreenButton, SIGNAL(released()), this, SLOT(handleLockscreenButton()));
    connect(sleepButton, SIGNAL(released()), this, SLOT(handleSleepButton()));
    connect(hibernateButton, SIGNAL(released()), this, SLOT(handleHibernateButton()));
    connect(poweroffButton, SIGNAL(released()), this, SLOT(handlePoweroffButton()));
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
    connect(autoSleepACAction, SIGNAL(currentIndexChanged(int)), this, SLOT(handleAutoSleepACAction(int)));
}

Dialog::~Dialog()
{
    Common::savePowerSettings("dialog_geometry", saveGeometry());
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

    autoSleepACAction->clear();
    autoSleepACAction->addItem(tr("None"), suspendNone);
    autoSleepACAction->addItem(tr("Sleep"), suspendSleep);
    autoSleepACAction->addItem(tr("Hibernate"), suspendHibernate);
    autoSleepACAction->addItem(tr("Shutdown"), suspendShutdown);

    lowBatteryAction->clear();
    lowBatteryAction->addItem(tr("Notify"), suspendNone);
    lowBatteryAction->addItem(tr("Sleep"), suspendSleep);
    lowBatteryAction->addItem(tr("Hibernate"), suspendHibernate);
    lowBatteryAction->addItem(tr("Shutdown"), suspendShutdown);

    monitorRotation->clear();
    monitorRotation->addItem(tr("Normal"), "normal");
    monitorRotation->addItem(tr("Left"), "left");
    monitorRotation->addItem(tr("Right"), "right");
    monitorRotation->addItem(tr("Inverted"), "inverted");

    monitorPosition->clear();
    monitorPosition->addItem(tr("Auto"), randrAuto);
    monitorPosition->addItem(tr("Left Of"), randrLeftOf);
    monitorPosition->addItem(tr("Right Of"), randrRightOf);
    monitorPosition->addItem(tr("Above"), randrAbove);
    monitorPosition->addItem(tr("Below"), randrBelow);
    monitorPosition->addItem(tr("Same As"), randrSameAs);

    handleUpdatedMonitors();
}

// load settings and set as default in widgets
void Dialog::loadSettings()
{
    if (Common::validPowerSettings("dialog_geometry")) {
        restoreGeometry(Common::loadPowerSettings("dialog_geometry").toByteArray());
    }

    int defaultLowBatteryAction = suspendNone;
    if (Common::validPowerSettings("low_battery_action")) {
        defaultLowBatteryAction = Common::loadPowerSettings("low_battery_action").toInt();
    }
    setDefaultAction(lowBatteryAction, defaultLowBatteryAction);

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

    int defaultAutoSleepACAction = DEFAULT_SUSPEND_AC_ACTION;
    if (Common::validPowerSettings("suspend_ac_action")) {
        defaultAutoSleepACAction = Common::loadPowerSettings("suspend_ac_action").toInt();
    }
    setDefaultAction(autoSleepACAction, defaultAutoSleepACAction);

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

    sleepButton->setEnabled(UPower::canSuspend());
    hibernateButton->setEnabled(UPower::canHibernate());
    poweroffButton->setEnabled(UPower::canPowerOff());
}

// tell power manager to update settings
void Dialog::updatePM()
{
    if (!dbus->isValid()) { return; }
    dbus->call("refresh");
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

// set default monitor rotation
void Dialog::setDefaultRotation(QString value)
{
    for (int i=0;i<monitorRotation->count();i++) {
        if (monitorRotation->itemData(i) == value) {
            monitorRotation->setCurrentIndex(i);
            return;
        }
    }
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
    // TODO: add restart now?
}

void Dialog::handleDesktopPM(bool triggered)
{
    Common::savePowerSettings("desktop_pm", triggered);
    updatePM();
    QMessageBox::information(this, tr("Restart required"), tr("You must restart the power daemon to apply this setting"));
    // TODO: add restart now?
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

void Dialog::handleAutoSleepACAction(int index)
{
    Common::savePowerSettings("suspend_ac_action", index);
    updatePM();
}

void Dialog::handleUpdatedMonitors()
{
    qDebug() << "monitors changed";
    QMap<QString,bool> map = Monitor::getX();
    QMapIterator<QString, bool> i(map);
    while (i.hasNext()) {
        i.next();
        if (monitorExists(i.key())) { continue; }
        QListWidgetItem *item = new QListWidgetItem(monitorList);
        item->setText(i.key());
        item->setData(MONITOR_DATA_CONNECTED, i.value());
        item->setIcon(QIcon::fromTheme("video-display", QIcon(":/icons/video-display.png")));
        monitorList->setCurrentItem(item);
    }
    for (int i=0;i<monitorList->count();++i) {
        QListWidgetItem *item = monitorList->item(i);
        if (!item) { continue; }
        if (map.contains(item->text())) { continue; }
        else {
            delete monitorList->takeItem(i);
        }
    }
}

void Dialog::handleLockscreenButton()
{
    QProcess::startDetached(XSCREENSAVER_LOCK);
}

void Dialog::handleSleepButton()
{
    if (UPower::canSuspend()) { UPower::suspend(); }
    else {
        QMessageBox::information(this,
                                 tr("Power Action"),
                                 tr("System denied power request."
                                    " Maybe the required daemon is not running,"
                                    " or you may not have the required permissions."));
    }
}

void Dialog::handleHibernateButton()
{
    if (UPower::canHibernate()) { UPower::hibernate(); }
    else {
        QMessageBox::information(this,
                                 tr("Power Action"),
                                 tr("System denied power request."
                                    " Maybe the required daemon is not running,"
                                    " or you may not have the required permissions."));
    }
}

void Dialog::handlePoweroffButton()
{
    if (UPower::canPowerOff()) { UPower::poweroff(); }
    else {
        QMessageBox::information(this,
                                 tr("Power Action"),
                                 tr("System denied power request."
                                    " Maybe the required daemon is not running,"
                                    " or you may not have the required permissions."));
    }
}

void Dialog::handleLowBatteryAction(int value)
{
    Common::savePowerSettings("low_battery_action", value);
    updatePM();
}

bool Dialog::monitorExists(QString display)
{
    for (int i=0;i<monitorList->count();++i) {
        QListWidgetItem *item = monitorList->item(i);
        if (!item) { continue; }
        if (item->text() == display) { return true; }
    }
    return false;
}

void Dialog::handleMonitorListItemChanged(QListWidgetItem *item)
{
    if (!item) { return; }

    monitorModes->clear();
    monitorRates->clear();
    monitorPrimary->setChecked(false);
    monitorPositionOther->clear();
    monitorPositionOther->addItem(tr("None"));

    currentMonitorInfo = Monitor::getMonitorInfo(item->text());
    monitorPrimary->setChecked(currentMonitorInfo.isPrimary);

    for (int i=0;i<currentMonitorInfo.modes.size();++i) {
        QString mode = currentMonitorInfo.modes.at(i).at(0);
        if (mode.isEmpty()) { continue; }
        monitorModes->addItem(mode);
    }
    for (int i=0;i<monitorList->count();++i) {
        QListWidgetItem *monitor = monitorList->item(i);
        if (!monitor) { continue; }
        if (monitor->text() == item->text() || monitor->text().isEmpty()) { continue; }
        monitorPositionOther->addItem(monitor->text());
    }

    int pos = randrAuto;
    QString posOther = tr("None");
    if (Common::validPowerSettings(QString("%1_option").arg(item->text()))) {
        pos = Common::loadPowerSettings(QString("%1_option").arg(item->text())).toInt();
    }
    if (Common::validPowerSettings(QString("%1_option_value").arg(item->text()))) {
        posOther = Common::loadPowerSettings(QString("%1_option_value").arg(item->text())).toString();
    }
    monitorPosition->setCurrentIndex(pos);
    monitorPositionOther->setCurrentIndex(monitorPositionOther->findText(posOther));
    setDefaultAction(monitorModes, currentMonitorInfo.currentMode);
    setDefaultRotation(currentMonitorInfo.rotate);
}

void Dialog::handleMonitorListICurrentitemChanged(QListWidgetItem *item, QListWidgetItem *item2)
{
    Q_UNUSED(item2)
    handleMonitorListItemChanged(item);
}

void Dialog::handleMonitorModeChanged(QString mode)
{
    if (mode.isEmpty()) { return; }
    monitorRates->clear();
    QString currentRate;
    for (int i=0;i<currentMonitorInfo.modes.size();++i) {
        if (currentMonitorInfo.modes.at(i).at(0) == mode) {
            QStringList rates = currentMonitorInfo.modes.at(i);
            for (int y=0;y<rates.size();++y) {
                QString rate = rates.at(y);
                if (rate.isEmpty() || rate == mode) { continue; }
                if (rate.contains("*")) { currentRate = rate.replace("*","");}
                if (rate.contains("+")) { rate.replace("+",""); }
                monitorRates->addItem(rate);
            }
        }
    }
    if (!currentRate.isEmpty()) {
        setDefaultAction(monitorRates, currentRate);
    }
}

void Dialog::monitorSaveSettings()
{
    QListWidgetItem *item = monitorList->currentItem();
    if (!item) { return; }
    if (item->text().isEmpty()) { return; }

    Common::savePowerSettings(QString("%1_mode").arg(item->text()), monitorModes->currentText());
    Common::savePowerSettings(QString("%1_rate").arg(item->text()), monitorRates->currentText());
    Common::savePowerSettings(QString("%1_rotate").arg(item->text()),monitorRotation->itemData(monitorRotation->currentIndex()).toString());
    Common::savePowerSettings(QString("%1_option").arg(item->text()), monitorPosition->itemData(monitorPosition->currentIndex()).toInt());
    Common::savePowerSettings(QString("%1_option_value").arg(item->text()), monitorPositionOther->currentText());
    //Common::savePowerSettings(QString("%1_").arg(item->text()),"");
}

void Dialog::monitorApplySettings()
{
    QListWidgetItem *item = monitorList->currentItem();
    if (!item) { return; }
    if (item->text().isEmpty()) { return; }
    QString xrandr = QString("%1 --output %2").arg(XRANDR).arg(item->text());
    if (!monitorModes->currentText().isEmpty()) {
        xrandr.append(QString(" --mode %1").arg(monitorModes->currentText()));
    }
    if (!monitorRates->currentText().isEmpty()) {
        xrandr.append(QString(" --rate %1").arg(monitorRates->currentText()));
    }
    if (monitorPrimary->isChecked()) {
        xrandr.append(" --primary");
    }
    if (!monitorRotation->currentText().isEmpty()) {
        xrandr.append(QString(" --rotate %1").arg(monitorRotation->itemData(monitorRotation->currentIndex()).toString()));
    }
    if (!monitorPosition->currentText().isEmpty() && !monitorPositionOther->currentText().isEmpty()) {
        QString pos;
        switch(monitorPosition->itemData(monitorPosition->currentIndex()).toInt()) {
        case randrLeftOf:
            pos.append(" --left-of ");
            break;
        case randrRightOf:
            pos.append(" --right-of ");
            break;
        case randrAbove:
            pos.append(" --above ");
            break;
        case randrBelow:
            pos.append(" --below ");
            break;
        case randrSameAs:
            pos.append(" --same-as ");
            break;
        default:
            break;
        }
        if (!pos.isEmpty() && monitorPositionOther->currentText() != tr("None")) {
            pos.append(monitorPositionOther->currentText());
            xrandr.append(pos);
        }
    }
    qDebug() << "run" << xrandr;
    QProcess proc;
    proc.start(xrandr);
    proc.waitForFinished();
    handleMonitorListItemChanged(item);
}
