/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , dbus(0)
    , lidActionBattery(0)
    , lidActionAC(0)
    , criticalActionBattery(0)
    , criticalBattery(0)
    , autoSleepBattery(0)
    , autoSleepAC(0)
    , desktopSS(0)
    , desktopPM(0)
    , showNotifications(0)
    , showSystemTray(0)
    , disableLidAction(0)
    , autoSleepBatteryAction(0)
    , autoSleepACAction(0)
    , lockscreenButton(0)
    , sleepButton(0)
    , hibernateButton(0)
    , poweroffButton(0)
//    , lidXrandr(0)
    , hasBacklight(false)
    , backlightSlider(0)
    , backlightWatcher(0)
    , man(0)
    , batteryIcon(0)
    , batteryLabel(0)
    , deviceTree(0)
    , batteryLeftLCD(0)
    , backlightSliderBattery(0)
    , backlightSliderAC(0)
    , backlightBatteryCheck(0)
    , backlightACCheck(0)
    , backlightBatteryLowerCheck(0)
    , backlightACHigherCheck(0)
    , inhibitorTree(0)
{
    // setup dialog
    setAttribute(Qt::WA_QuitOnClose, true);
    setWindowTitle(tr("PowerKit"));
    setMinimumSize(QSize(390, 310));

    // setup dbus
    QDBusConnection session = QDBusConnection::sessionBus();
    dbus = new QDBusInterface(POWERKIT_SERVICE,
                              POWERKIT_PATH,
                              POWERKIT_SERVICE,
                              session, this);
    session.connect(dbus->service(),
                   dbus->path(),
                   dbus->service(),
                   "UpdatedInhibitors",
                   this,
                   SLOT(handleUpdatedInhibitors()));

    // setup powerkit
    man = new PowerKit(this);

    // setup theme
    Common::setIconTheme();
    setWindowIcon(QIcon::fromTheme(DEFAULT_AC_ICON));

    // setup widgets
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(5);
    layout->setSpacing(0);

    QTabWidget *containerWidget = new QTabWidget(this);

    QWidget *wrapper = new QWidget(this);
    wrapper->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *wrapperLayout = new QVBoxLayout(wrapper);
    wrapperLayout->setMargin(0);
    wrapperLayout->setSpacing(0);
    wrapperLayout->addWidget(containerWidget);

    // battery
    QGroupBox *batteryContainer = new QGroupBox(this);
    batteryContainer->setTitle(tr("On Battery"));
    batteryContainer->setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Expanding);
    QVBoxLayout *batteryContainerLayout = new QVBoxLayout(batteryContainer);
    batteryContainerLayout->setMargin(0);
    batteryContainerLayout->setSpacing(0);

    QWidget *lidActionBatteryContainer = new QWidget(this);
    QHBoxLayout *lidActionBatteryContainerLayout = new QHBoxLayout(lidActionBatteryContainer);
    lidActionBattery = new QComboBox(this);
    lidActionBattery->setMaximumWidth(MAX_WIDTH);
    lidActionBattery->setMinimumWidth(MAX_WIDTH);
    QLabel *lidActionBatteryLabel = new QLabel(this);

    QLabel *lidActionBatteryIcon = new QLabel(this);
    lidActionBatteryIcon->setMaximumSize(48, 48);
    lidActionBatteryIcon->setMinimumSize(48, 48);
    lidActionBatteryIcon->setPixmap(QIcon::fromTheme(DEFAULT_VIDEO_ICON)
                                    .pixmap(QSize(48, 48)));
    lidActionBatteryLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>")
                                   .arg(tr("Lid action")));
    lidActionBatteryLabel->setToolTip(tr("What to do when the lid is closed and running on battery."));
    lidActionBatteryContainerLayout->addWidget(lidActionBatteryIcon);
    lidActionBatteryContainerLayout->addWidget(lidActionBatteryLabel);
    lidActionBatteryContainerLayout->addStretch();
    lidActionBatteryContainerLayout->addWidget(lidActionBattery);
    batteryContainerLayout->addWidget(lidActionBatteryContainer);

    QWidget *criticalBatteryContainer = new QWidget(this);
    QHBoxLayout *criticalBatteryContainerLayout = new QHBoxLayout(criticalBatteryContainer);
    criticalBattery = new QSpinBox(this);
    criticalBattery->setMaximumWidth(MAX_WIDTH);
    criticalBattery->setMinimumWidth(MAX_WIDTH);
    criticalBattery->setMinimum(0);
    criticalBattery->setMaximum(99);
    criticalBattery->setSuffix(tr(" %"));
    QLabel *criticalBatteryLabel = new QLabel(this);

    QWidget *criticalActionBatteryContainer = new QWidget(this);
    criticalActionBatteryContainer->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *criticalActionBatteryContainerLayout = new QVBoxLayout(criticalActionBatteryContainer);
    criticalActionBatteryContainerLayout->setMargin(0);
    criticalActionBatteryContainerLayout->setSpacing(0);
    criticalActionBattery = new QComboBox(this);
    criticalActionBattery->setMaximumWidth(MAX_WIDTH);
    criticalActionBattery->setMinimumWidth(MAX_WIDTH);
    criticalActionBatteryContainerLayout->addWidget(criticalBattery);
    criticalActionBatteryContainerLayout->addWidget(criticalActionBattery);

    QLabel *criticalBatteryIcon = new QLabel(this);
    criticalBatteryIcon->setMaximumSize(48, 48);
    criticalBatteryIcon->setMinimumSize(48, 48);
    criticalBatteryIcon->setPixmap(QIcon::fromTheme(DEFAULT_BATTERY_ICON_CRIT)
                                   .pixmap(QSize(48, 48)));
    criticalBatteryLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>")
                                  .arg(tr("Critical battery")));
    criticalBatteryLabel->setToolTip(tr("What to do when your battery is critical."));
    criticalBatteryContainerLayout->addWidget(criticalBatteryIcon);
    criticalBatteryContainerLayout->addWidget(criticalBatteryLabel);
    criticalBatteryContainerLayout->addStretch();
    criticalBatteryContainerLayout->addWidget(criticalActionBatteryContainer);

    QWidget *sleepBatteryContainer = new QWidget(this);
    QHBoxLayout *sleepBatteryContainerLayout = new QHBoxLayout(sleepBatteryContainer);
    autoSleepBattery = new QSpinBox(this);
    autoSleepBattery->setMaximumWidth(MAX_WIDTH);
    autoSleepBattery->setMinimumWidth(MAX_WIDTH);
    autoSleepBattery->setMinimum(0);
    autoSleepBattery->setMaximum(1000);
    autoSleepBattery->setSuffix(QString(" %1").arg(tr("min")));
    QLabel *sleepBatteryLabel = new QLabel(this);

    QWidget *sleepActionBatteryContainer = new QWidget(this);
    sleepActionBatteryContainer->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *sleepActionBatteryContainerLayout = new QVBoxLayout(sleepActionBatteryContainer);
    sleepActionBatteryContainerLayout->setMargin(0);
    sleepActionBatteryContainerLayout->setSpacing(0);
    autoSleepBatteryAction = new QComboBox(this);
    autoSleepBatteryAction->setMaximumWidth(MAX_WIDTH);
    autoSleepBatteryAction->setMinimumWidth(MAX_WIDTH);
    sleepActionBatteryContainerLayout->addWidget(autoSleepBattery);
    sleepActionBatteryContainerLayout->addWidget(autoSleepBatteryAction);

    QLabel *sleepBatteryIcon = new QLabel(this);
    sleepBatteryIcon->setMaximumSize(48, 48);
    sleepBatteryIcon->setMinimumSize(48, 48);
    sleepBatteryIcon->setPixmap(QIcon::fromTheme(DEFAULT_SUSPEND_ICON)
                                .pixmap(QSize(48, 48)));
    sleepBatteryLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>")
                               .arg(tr("Suspend after")));
    sleepBatteryLabel->setToolTip(tr("Enable automatically suspend when on battery."));
    sleepBatteryContainerLayout->addWidget(sleepBatteryIcon);
    sleepBatteryContainerLayout->addWidget(sleepBatteryLabel);
    sleepBatteryContainerLayout->addStretch();
    sleepBatteryContainerLayout->addWidget(sleepActionBatteryContainer);

    // backlight battery
    backlightSliderBattery = new QSlider(this);
    backlightSliderBattery->setOrientation(Qt::Horizontal);
    backlightSliderBattery->setMinimum(1);
    backlightSliderBattery->setMaximum(1);
    backlightSliderBattery->setValue(0);
    backlightSliderBattery->setMaximumWidth(MAX_WIDTH);

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

    QWidget *batteryBacklightOptContainer = new QWidget(this);
    QVBoxLayout *batteryBacklightOptContainerLayout = new QVBoxLayout(batteryBacklightOptContainer);
    batteryBacklightOptContainer->setContentsMargins(0, 0, 0, 0);
    batteryBacklightOptContainer->setMaximumWidth(MAX_WIDTH);
    batteryBacklightOptContainerLayout->setMargin(0);
    batteryBacklightOptContainerLayout->setContentsMargins(0, 0, 0, 0);
    batteryBacklightOptContainerLayout->addWidget(backlightSliderBattery);
    batteryBacklightOptContainerLayout->addWidget(backlightBatteryLowerCheck);

    QWidget *batteryBacklightContainer = new QWidget(this);
    QHBoxLayout *batteryBacklightContainerLayout = new QHBoxLayout(batteryBacklightContainer);
    QLabel *batteryBacklightLabel = new QLabel(this);
    QLabel *batteryBacklightIcon = new QLabel(this);

    batteryBacklightIcon->setMaximumSize(48, 48);
    batteryBacklightIcon->setMinimumSize(48, 48);
    batteryBacklightIcon->setPixmap(QIcon::fromTheme(DEFAULT_BACKLIGHT_ICON)
                                .pixmap(QSize(48, 48)));
    batteryBacklightLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>")
                                   .arg(tr("Brightness")));
    batteryBacklightLabel->setToolTip(tr("Override brightness when switched to battery power."));
    batteryBacklightContainerLayout->addWidget(batteryBacklightIcon);
    batteryBacklightContainerLayout->addWidget(batteryBacklightLabel);
    batteryBacklightContainerLayout->addWidget(backlightBatteryCheck);
    batteryBacklightContainerLayout->addStretch();
    batteryBacklightContainerLayout->addWidget(batteryBacklightOptContainer);

    // add battery widgets to container
    batteryContainerLayout->addWidget(sleepBatteryContainer);
    batteryContainerLayout->addWidget(criticalBatteryContainer);
    batteryContainerLayout->addWidget(batteryBacklightContainer);
    batteryContainerLayout->addStretch();

    // AC
    QGroupBox *acContainer = new QGroupBox(this);
    acContainer->setTitle(tr("On AC"));
    QVBoxLayout *acContainerLayout = new QVBoxLayout(acContainer);
    acContainerLayout->setMargin(0);
    acContainerLayout->setSpacing(0);

    QWidget *lidActionACContainer = new QWidget(this);
    QHBoxLayout *lidActionACContainerLayout = new QHBoxLayout(lidActionACContainer);

    lidActionAC = new QComboBox(this);
    lidActionAC->setMaximumWidth(MAX_WIDTH);
    lidActionAC->setMinimumWidth(MAX_WIDTH);
    QLabel *lidActionACLabel = new QLabel(this);

    QLabel *lidActionACIcon = new QLabel(this);
    lidActionACIcon->setMaximumSize(48, 48);
    lidActionACIcon->setMinimumSize(48, 48);
    lidActionACIcon->setPixmap(QIcon::fromTheme(DEFAULT_VIDEO_ICON)
                               .pixmap(QSize(48, 48)));
    lidActionACLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>")
                              .arg(tr("Lid action")));
    lidActionACLabel->setToolTip(tr("What to do when the lid is closed and running on AC."));
    lidActionACContainerLayout->addWidget(lidActionACIcon);
    lidActionACContainerLayout->addWidget(lidActionACLabel);
    lidActionACContainerLayout->addStretch();
    lidActionACContainerLayout->addWidget(lidActionAC);
    acContainerLayout->addWidget(lidActionACContainer);

    QWidget *sleepACContainer = new QWidget(this);
    QHBoxLayout *sleepACContainerLayout = new QHBoxLayout(sleepACContainer);
    autoSleepAC = new QSpinBox(this);
    autoSleepAC->setMaximumWidth(MAX_WIDTH);
    autoSleepAC->setMinimumWidth(MAX_WIDTH);
    autoSleepAC->setMinimum(0);
    autoSleepAC->setMaximum(1000);
    autoSleepAC->setSuffix(QString(" %1").arg(tr("min")));
    QLabel *sleepACLabel = new QLabel(this);

    sleepACContainerLayout->addWidget(sleepACLabel);
    acContainerLayout->addWidget(sleepACContainer);

    QWidget *sleepActionACContainer = new QWidget(this);
    sleepActionACContainer->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *sleepActionACContainerLayout = new QVBoxLayout(sleepActionACContainer);
    sleepActionACContainerLayout->setMargin(0);
    sleepActionACContainerLayout->setSpacing(0);
    autoSleepACAction = new QComboBox(this);
    autoSleepACAction->setMaximumWidth(MAX_WIDTH);
    autoSleepACAction->setMinimumWidth(MAX_WIDTH);
    sleepActionACContainerLayout->addWidget(autoSleepAC);
    sleepActionACContainerLayout->addWidget(autoSleepACAction);

    QLabel *sleepACIcon = new QLabel(this);
    sleepACIcon->setMaximumSize(48, 48);
    sleepACIcon->setMinimumSize(48, 48);
    sleepACIcon->setPixmap(QIcon::fromTheme(DEFAULT_SUSPEND_ICON)
                           .pixmap(QSize(48, 48)));
    sleepACLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>")
                          .arg(tr("Suspend after")));
    sleepACLabel->setToolTip(tr("Enable automatically suspend when on AC."));
    sleepACContainerLayout->addWidget(sleepACIcon);
    sleepACContainerLayout->addWidget(sleepACLabel);
    sleepACContainerLayout->addStretch();
    sleepACContainerLayout->addWidget(sleepActionACContainer);

    // backlight ac
    backlightSliderAC = new QSlider(this);
    backlightSliderAC->setOrientation(Qt::Horizontal);
    backlightSliderAC->setMinimum(1);
    backlightSliderAC->setMaximum(1);
    backlightSliderAC->setValue(0);
    backlightSliderAC->setMaximumWidth(MAX_WIDTH);

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

    QWidget *acBacklightOptContainer = new QWidget(this);
    QVBoxLayout *acBacklightOptContainerLayout = new QVBoxLayout(acBacklightOptContainer);
    acBacklightOptContainer->setContentsMargins(0, 0, 0, 0);
    acBacklightOptContainer->setMaximumWidth(MAX_WIDTH);
    acBacklightOptContainerLayout->setMargin(0);
    acBacklightOptContainerLayout->setContentsMargins(0, 0, 0, 0);
    acBacklightOptContainerLayout->addWidget(backlightSliderAC);
    acBacklightOptContainerLayout->addWidget(backlightACHigherCheck);

    QWidget *acBacklightContainer = new QWidget(this);
    QHBoxLayout *acBacklightContainerLayout = new QHBoxLayout(acBacklightContainer);
    QLabel *acBacklightLabel = new QLabel(this);
    QLabel *acBacklightIcon = new QLabel(this);

    acBacklightIcon->setMaximumSize(48, 48);
    acBacklightIcon->setMinimumSize(48, 48);
    acBacklightIcon->setPixmap(QIcon::fromTheme(DEFAULT_BACKLIGHT_ICON)
                                .pixmap(QSize(48, 48)));
    acBacklightLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>")
                              .arg(tr("Brightness")));
    acBacklightLabel->setToolTip(tr("Override brightness when switched to AC power."));
    acBacklightContainerLayout->addWidget(acBacklightIcon);
    acBacklightContainerLayout->addWidget(acBacklightLabel);
    acBacklightContainerLayout->addWidget(backlightACCheck);
    acBacklightContainerLayout->addStretch();
    acBacklightContainerLayout->addWidget(acBacklightOptContainer);

    // add widgets to ac
    acContainerLayout->addWidget(sleepACContainer);
    acContainerLayout->addWidget(acBacklightContainer);
    acContainerLayout->addStretch();

    // advanced
    QGroupBox *advContainer = new QGroupBox(this);
    advContainer->setTitle(tr("Advanced"));
    QVBoxLayout *advContainerLayout = new QVBoxLayout(advContainer);

    showSystemTray  = new QCheckBox(this);
    showSystemTray->setIcon(QIcon::fromTheme(DEFAULT_TRAY_ICON));
    showSystemTray->setText(tr("Show system tray"));
    showSystemTray->setToolTip(tr("Enable/Disable the system tray icon."
                                  " Note that notifications will not work when the systemtray is disabled."));

    showNotifications = new QCheckBox(this);
    showNotifications->setIcon(QIcon::fromTheme(DEFAULT_NOTIFY_ICON));
    showNotifications->setText(tr("Show notifications"));
    showNotifications->setToolTip(tr("Show notifications for power related events."));

    desktopSS = new QCheckBox(this);
    desktopSS->setIcon(QIcon::fromTheme(DEFAULT_VIDEO_ICON));
    desktopSS->setText("org.freedesktop.ScreenSaver");
    desktopSS->setToolTip(tr("Enable/Disable the screen saver D-Bus service."
                             " Needed for applications to inhibit the running screen saver."));

    desktopPM = new QCheckBox(this);
    desktopPM->setIcon(QIcon::fromTheme(DEFAULT_BATTERY_ICON));
    desktopPM->setText("org.freedesktop.PowerManagement");
    desktopPM->setToolTip(tr("Enable/Disable the power management D-Bus service."
                             " Needed for applications to inhibit auto suspend action."));

    /*lidXrandr = new QCheckBox(this);
    lidXrandr->setIcon(QIcon::fromTheme(DEFAULT_VIDEO_ICON));
    lidXrandr->setText(tr("Switch internal monitor on/off"
                          "\nwith xrandr if lid action disabled"));*/

    disableLidAction = new QCheckBox(this);
    disableLidAction->setIcon(QIcon::fromTheme(DEFAULT_VIDEO_ICON));
    disableLidAction->setText(tr("Disable lid action if external"
                                 "\nmonitor(s) is connected"));
    disableLidAction->setToolTip(tr("If an external monitor (HDMI/VGA/DVI/DP etc) is connected"
                                    " to your laptop, disable/enable lid action."));

    // add widgets to advanced
    advContainerLayout->addWidget(showSystemTray);
    advContainerLayout->addWidget(showNotifications);
    advContainerLayout->addWidget(disableLidAction);
    advContainerLayout->addWidget(desktopSS);
    advContainerLayout->addWidget(desktopPM);
    //advContainerLayout->addWidget(lidXrandr);
    advContainerLayout->addStretch();

    // extra
    QWidget *extraContainer = new QWidget(this);
    QHBoxLayout *extraContainerLayout = new QHBoxLayout(extraContainer);

    lockscreenButton = new QPushButton(this);
    lockscreenButton->setIcon(QIcon::fromTheme(DEFAULT_LOCK_ICON));
    lockscreenButton->setIconSize(QSize(24, 24));
    lockscreenButton->setToolTip(tr("Lock the screen now."));
    if (lockscreenButton->icon().isNull()) {
        lockscreenButton->setText(tr("Lock screen"));
    }

    sleepButton = new QPushButton(this);
    sleepButton->setIcon(QIcon::fromTheme(DEFAULT_SUSPEND_ICON));
    sleepButton->setIconSize(QSize(24, 24));
    sleepButton->setToolTip(tr("Suspend computer now."));
    if (sleepButton->icon().isNull()) {
        sleepButton->setText(tr("Suspend"));
    }

    hibernateButton = new QPushButton(this);
    hibernateButton->setIcon(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON));
    hibernateButton->setIconSize(QSize(24, 24));
    hibernateButton->setToolTip(tr("Hibernate computer now."));
    if (hibernateButton->icon().isNull()) {
        hibernateButton->setText(tr("Hibernate"));
    }

    poweroffButton = new QPushButton(this);
    poweroffButton->setIcon(QIcon::fromTheme(DEFAULT_SHUTDOWN_ICON));
    poweroffButton->setIconSize(QSize(24, 24));
    poweroffButton->setToolTip(tr("Shutdown computer now."));
    if (poweroffButton->icon().isNull()) {
        poweroffButton->setText(tr("Shutdown"));
    }

    backlightSlider = new QSlider(this);
    backlightSlider->setMinimumWidth(100);
    backlightSlider->setSingleStep(1);
    backlightSlider->setOrientation(Qt::Horizontal);
    backlightSlider->setToolTip(tr("Adjust the current brightness."));
    backlightWatcher = new QFileSystemWatcher(this);

    QLabel *backlightLabel = new QLabel(this);
    backlightLabel->setPixmap(QIcon::fromTheme(DEFAULT_BACKLIGHT_ICON)
                              .pixmap(24, 24));

    extraContainerLayout->addWidget(backlightLabel);
    extraContainerLayout->addWidget(backlightSlider);
    extraContainerLayout->addStretch();
    extraContainerLayout->addWidget(lockscreenButton);
    extraContainerLayout->addWidget(sleepButton);
    extraContainerLayout->addWidget(hibernateButton);
    extraContainerLayout->addWidget(poweroffButton);

    // status
    QWidget *statusContainer = new QWidget(this);
    QVBoxLayout *statusContainerLayout = new QVBoxLayout(statusContainer);

    QGroupBox *batteryStatusBox = new QGroupBox(this);
    batteryStatusBox->setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Fixed);
    QHBoxLayout *batteryStatusLayout = new QHBoxLayout(batteryStatusBox);

    batteryIcon = new QLabel(this);
    batteryLabel = new QLabel(this);
    batteryIcon->setPixmap(QIcon::fromTheme(DEFAULT_BATTERY_ICON)
                           .pixmap(QSize(48, 48)));

    batteryLeftLCD = new QLCDNumber(this);
    batteryLeftLCD->setSizePolicy(QSizePolicy::Expanding,
                                  QSizePolicy::Expanding);
    batteryLeftLCD->setFrameStyle(QFrame::NoFrame);
    batteryLeftLCD->setSegmentStyle(QLCDNumber::Flat);
    batteryLeftLCD->display("00:00");

    deviceTree = new QTreeWidget(this);
    deviceTree->setStyleSheet("QTreeWidget,QTreeWidget::item,"
                              "QTreeWidget::item:selected"
                              "{background:transparent;border:0;}");
    deviceTree->setHeaderHidden(true);
    deviceTree->setHeaderLabels(QStringList() << "1" << "2");
    deviceTree->setColumnWidth(0, 150);

    QLabel *aboutLabel = new QLabel(this);
    aboutLabel->setText(QString("<p style=\"font-size:small;\">"
                                "<a href=\"https://github.com/rodlie/powerkit\">"
                                "PowerKit</a> %1 &copy;2018 Ole-Andr&eacute; Rodlie")
                        .arg(qApp->applicationVersion()));

    batteryStatusLayout->addWidget(batteryIcon);
    batteryStatusLayout->addWidget(batteryLabel);
    batteryStatusLayout->addStretch();
    batteryStatusLayout->addWidget(batteryLeftLCD);

    statusContainerLayout->addWidget(batteryStatusBox);
    statusContainerLayout->addWidget(deviceTree);
    statusContainerLayout->addStretch();
    statusContainerLayout->addWidget(aboutLabel);

    layout->addWidget(wrapper);
    layout->addWidget(extraContainer);

    QWidget *settingsWidget = new QWidget(this);
    QVBoxLayout *settingsLayout = new QVBoxLayout(settingsWidget);
    QScrollArea *settingsContainerArea = new QScrollArea(this);
    settingsContainerArea->setSizePolicy(QSizePolicy::Expanding,
                                         QSizePolicy::Expanding);
    settingsContainerArea->setStyleSheet("QScrollArea {border:0;}");
    settingsContainerArea->setWidgetResizable(true);
    settingsContainerArea->setWidget(settingsWidget);

    // add widgets to settings
    settingsLayout->addWidget(batteryContainer);
    settingsLayout->addWidget(acContainer);
    //settingsLayout->addWidget(backlightContainer);
    settingsLayout->addWidget(advContainer);
    settingsLayout->addStretch();

    // inhibitors
    inhibitorTree = new QTreeWidget(this);
    inhibitorTree->setHeaderHidden(true);

    // add tabs
    containerWidget->addTab(statusContainer,
                            QIcon::fromTheme(DEFAULT_INFO_ICON),
                            tr("Status"));
    containerWidget->addTab(settingsContainerArea,
                            QIcon::fromTheme(DEFAULT_BATTERY_ICON),
                            tr("Settings"));
    containerWidget->addTab(inhibitorTree,
                            QIcon::fromTheme(DEFAULT_VIDEO_ICON),
                            tr("Inhibitors"));

    populate(); // populate boxes
    loadSettings(); // load settings

    // connect widgets
    connect(lockscreenButton, SIGNAL(released()),
            this, SLOT(handleLockscreenButton()));
    connect(sleepButton, SIGNAL(released()),
            this, SLOT(handleSleepButton()));
    connect(hibernateButton, SIGNAL(released()),
            this, SLOT(handleHibernateButton()));
    connect(poweroffButton, SIGNAL(released()),
            this, SLOT(handlePoweroffButton()));
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
    connect(desktopSS, SIGNAL(toggled(bool)),
            this, SLOT(handleDesktopSS(bool)));
    connect(desktopPM, SIGNAL(toggled(bool)),
            this, SLOT(handleDesktopPM(bool)));
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
    /*connect(lidXrandr, SIGNAL(toggled(bool)),
            this, SLOT(handleLidXrandr(bool)));*/
    connect(backlightSlider, SIGNAL(valueChanged(int)),
            this, SLOT(handleBacklightSlider(int)));
    connect(backlightWatcher, SIGNAL(fileChanged(QString)),
            this, SLOT(updateBacklight(QString)));
    connect(man, SIGNAL(UpdatedDevices()),
            this, SLOT(checkDevices()));
    connect(man, SIGNAL(DeviceWasRemoved(QString)),
            this, SLOT(deviceRemove(QString)));
    connect(man, SIGNAL(DeviceWasAdded(QString)),
            this, SLOT(handleDeviceAdded(QString)));
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
}

Dialog::~Dialog()
{
    saveSettings();
}

// populate widgets with default values
void Dialog::populate()
{
    lidActionBattery->clear();
    lidActionBattery->addItem(QIcon::fromTheme(DEFAULT_NONE_ICON),
                              tr("None"), lidNone);
    lidActionBattery->addItem(QIcon::fromTheme(DEFAULT_LOCK_ICON),
                              tr("Lock Screen"), lidLock);
    lidActionBattery->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                              tr("Sleep"), lidSleep);
    lidActionBattery->addItem(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON),
                              tr("Hibernate"), lidHibernate);
    lidActionBattery->addItem(QIcon::fromTheme(DEFAULT_SHUTDOWN_ICON),
                              tr("Shutdown"), lidShutdown);
    lidActionBattery->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                              tr("Hybrid Sleep"), lidHybridSleep);

    lidActionAC->clear();
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_NONE_ICON),
                         tr("None"), lidNone);
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_LOCK_ICON),
                         tr("Lock Screen"), lidLock);
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                         tr("Sleep"), lidSleep);
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON),
                         tr("Hibernate"), lidHibernate);
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_SHUTDOWN_ICON),
                         tr("Shutdown"), lidShutdown);
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                         tr("Hybrid Sleep"), lidHybridSleep);

    criticalActionBattery->clear();
    criticalActionBattery->addItem(QIcon::fromTheme(DEFAULT_NONE_ICON),
                                   tr("None"), criticalNone);
    criticalActionBattery->addItem(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON),
                                   tr("Hibernate"), criticalHibernate);
    criticalActionBattery->addItem(QIcon::fromTheme(DEFAULT_SHUTDOWN_ICON),
                                   tr("Shutdown"), criticalShutdown);

    autoSleepBatteryAction->clear();
    autoSleepBatteryAction->addItem(QIcon::fromTheme(DEFAULT_NONE_ICON),
                                    tr("None"), suspendNone);
    autoSleepBatteryAction->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                                    tr("Sleep"), suspendSleep);
    autoSleepBatteryAction->addItem(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON),
                                    tr("Hibernate"), suspendHibernate);
    autoSleepBatteryAction->addItem(QIcon::fromTheme(DEFAULT_SHUTDOWN_ICON),
                                    tr("Shutdown"), suspendShutdown);
    autoSleepBatteryAction->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                                    tr("Hybrid Sleep"), suspendHybrid);

    autoSleepACAction->clear();
    autoSleepACAction->addItem(QIcon::fromTheme(DEFAULT_NONE_ICON),
                               tr("None"), suspendNone);
    autoSleepACAction->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                               tr("Sleep"), suspendSleep);
    autoSleepACAction->addItem(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON),
                               tr("Hibernate"), suspendHibernate);
    autoSleepACAction->addItem(QIcon::fromTheme(DEFAULT_SHUTDOWN_ICON),
                               tr("Shutdown"), suspendShutdown);
    autoSleepACAction->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                               tr("Hybrid Sleep"), suspendHybrid);
}

// load settings and set defaults
void Dialog::loadSettings()
{
    if (Common::validPowerSettings(CONF_DIALOG_GEOMETRY)) {
        restoreGeometry(Common::loadPowerSettings(CONF_DIALOG_GEOMETRY).toByteArray());
    }

    int defaultAutoSleepBattery = AUTO_SLEEP_BATTERY;
    if (Common::validPowerSettings(CONF_SUSPEND_BATTERY_TIMEOUT)) {
        defaultAutoSleepBattery = Common::loadPowerSettings(CONF_SUSPEND_BATTERY_TIMEOUT).toInt();
    }
    setDefaultAction(autoSleepBattery, defaultAutoSleepBattery);

    int defaultAutoSleepBatteryAction = DEFAULT_SUSPEND_BATTERY_ACTION;
    if (Common::validPowerSettings(CONF_SUSPEND_BATTERY_ACTION)) {
        defaultAutoSleepBatteryAction = Common::loadPowerSettings(CONF_SUSPEND_BATTERY_ACTION).toInt();
    }
    setDefaultAction(autoSleepBatteryAction, defaultAutoSleepBatteryAction);

    int defaultAutoSleepAC = 0;
    if (Common::validPowerSettings(CONF_SUSPEND_AC_TIMEOUT)) {
        defaultAutoSleepAC = Common::loadPowerSettings(CONF_SUSPEND_AC_TIMEOUT).toInt();
    }
    setDefaultAction(autoSleepAC, defaultAutoSleepAC);

    int defaultAutoSleepACAction = DEFAULT_SUSPEND_AC_ACTION;
    if (Common::validPowerSettings(CONF_SUSPEND_AC_ACTION)) {
        defaultAutoSleepACAction = Common::loadPowerSettings(CONF_SUSPEND_AC_ACTION).toInt();
    }
    setDefaultAction(autoSleepACAction, defaultAutoSleepACAction);

    int defaultCriticalBattery = CRITICAL_BATTERY;
    if (Common::validPowerSettings(CONF_CRITICAL_BATTERY_TIMEOUT)) {
        defaultCriticalBattery = Common::loadPowerSettings(CONF_CRITICAL_BATTERY_TIMEOUT).toInt();
    }
    setDefaultAction(criticalBattery, defaultCriticalBattery);

    int defaultLidActionBattery = LID_BATTERY_DEFAULT;
    if (Common::validPowerSettings(CONF_LID_BATTERY_ACTION)) {
        defaultLidActionBattery = Common::loadPowerSettings(CONF_LID_BATTERY_ACTION).toInt();
    }
    setDefaultAction(lidActionBattery, defaultLidActionBattery);

    int defaultLidActionAC = LID_AC_DEFAULT;
    if (Common::validPowerSettings(CONF_LID_AC_ACTION)) {
        defaultLidActionAC = Common::loadPowerSettings(CONF_LID_AC_ACTION).toInt();
    }
    setDefaultAction(lidActionAC, defaultLidActionAC);

    int defaultCriticalAction = CRITICAL_DEFAULT;
    if (Common::validPowerSettings(CONF_CRITICAL_BATTERY_ACTION)) {
        defaultCriticalAction = Common::loadPowerSettings(CONF_CRITICAL_BATTERY_ACTION).toInt();
    }
    setDefaultAction(criticalActionBattery, defaultCriticalAction);

    bool defaultDesktopSS = true;
    if (Common::validPowerSettings(CONF_FREEDESKTOP_SS)) {
        defaultDesktopSS = Common::loadPowerSettings(CONF_FREEDESKTOP_SS).toBool();
    }
    desktopSS->setChecked(defaultDesktopSS);

    bool defaultDesktopPM = true;
    if (Common::validPowerSettings(CONF_FREEDESKTOP_PM)) {
        defaultDesktopPM = Common::loadPowerSettings(CONF_FREEDESKTOP_PM).toBool();
    }
    desktopPM->setChecked(defaultDesktopPM);

    /*bool defaultLidXrandr = false;
    if (Common::validPowerSettings(CONF_LID_XRANDR)) {
        defaultLidXrandr = Common::loadPowerSettings(CONF_LID_XRANDR).toBool();
    }
    lidXrandr->setChecked(defaultLidXrandr);*/

    bool defaultShowNotifications = true;
    if (Common::validPowerSettings(CONF_TRAY_NOTIFY)) {
        defaultShowNotifications = Common::loadPowerSettings(CONF_TRAY_NOTIFY).toBool();
    }
    showNotifications->setChecked(defaultShowNotifications);

    bool defaultShowTray = true;
    if (Common::validPowerSettings(CONF_TRAY_SHOW)) {
        defaultShowTray = Common::loadPowerSettings(CONF_TRAY_SHOW).toBool();
    }
    showSystemTray->setChecked(defaultShowTray);

    bool defaultDisableLidAction = true;
    if (Common::validPowerSettings(CONF_LID_DISABLE_IF_EXTERNAL)) {
        defaultDisableLidAction = Common::loadPowerSettings(CONF_LID_DISABLE_IF_EXTERNAL).toBool();
    }
    disableLidAction->setChecked(defaultDisableLidAction);

    // power actions
    bool canSuspend = man->CanSuspend();
    bool canHibernate = man->CanHibernate();
    bool canShutdown = man->CanPowerOff();
    QString notSupported = tr("%1 is not supported. Check permissions and/or settings.");
    sleepButton->setEnabled(canSuspend);
    hibernateButton->setEnabled(canHibernate &&
                                Common::kernelCanResume());
    poweroffButton->setEnabled(canShutdown);
    if (!canSuspend) {
        sleepButton->setToolTip(notSupported.arg(tr("Suspend")));
    }
    if (!canHibernate) {
        hibernateButton->setToolTip(notSupported.arg(tr("Hibernate")));
    }
    if (!canShutdown) {
        poweroffButton->setToolTip(notSupported.arg(tr("Shutdown")));
    }

    checkPerms();

    // backlight
    backlightDevice = Common::backlightDevice();
    hasBacklight = Common::canAdjustBacklight(backlightDevice);
    if (hasBacklight) {
        backlightSlider->setMinimum(1);
        backlightSlider->setMaximum(Common::backlightMax(backlightDevice));
        backlightSlider->setValue(Common::backlightValue(backlightDevice));

        backlightSlider->setEnabled(true);
        backlightSliderAC->setEnabled(true);
        backlightSliderBattery->setEnabled(true);

        backlightWatcher->addPath(QString("%1/brightness").arg(backlightDevice));
        backlightSliderBattery->setMinimum(backlightSlider->minimum());
        backlightSliderBattery->setMaximum(backlightSlider->maximum());
        backlightSliderBattery->setValue(backlightSliderBattery->maximum());
        backlightSliderAC->setMinimum(backlightSlider->minimum());
        backlightSliderAC->setMaximum(backlightSlider->maximum());
        backlightSliderAC->setValue(backlightSliderAC->maximum());
    } else {
        backlightSlider->setDisabled(true);
        backlightSliderAC->setDisabled(true);
        backlightSliderBattery->setDisabled(true);
    }
    backlightBatteryCheck->setChecked(Common::loadPowerSettings(CONF_BACKLIGHT_BATTERY_ENABLE)
                                      .toBool());
    backlightACCheck->setChecked(Common::loadPowerSettings(CONF_BACKLIGHT_AC_ENABLE)
                                 .toBool());
    if (Common::validPowerSettings(CONF_BACKLIGHT_BATTERY)) {
        backlightSliderBattery->setValue(Common::loadPowerSettings(CONF_BACKLIGHT_BATTERY)
                                         .toInt());
    }
    if (Common::validPowerSettings(CONF_BACKLIGHT_AC)) {
        backlightSliderAC->setValue(Common::loadPowerSettings(CONF_BACKLIGHT_AC)
                                    .toInt());
    }
    if (Common::validPowerSettings(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER)) {
        backlightBatteryLowerCheck->setChecked(
                    Common::loadPowerSettings(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER)
                    .toBool());
    }
    if (Common::validPowerSettings(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER)) {
        backlightACHigherCheck->setChecked(
                    Common::loadPowerSettings(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER)
                    .toBool());
    }
    enableBacklight(hasBacklight);

    // check devices
    checkDevices();

    // check inhibitors
    getInhibitors();
}

void Dialog::saveSettings()
{
    Common::savePowerSettings(CONF_LID_BATTERY_ACTION,
                              lidActionBattery->currentIndex());
    Common::savePowerSettings(CONF_LID_AC_ACTION,
                              lidActionAC->currentIndex());
    Common::savePowerSettings(CONF_CRITICAL_BATTERY_ACTION,
                              criticalActionBattery->currentIndex());
    Common::savePowerSettings(CONF_CRITICAL_BATTERY_TIMEOUT,
                              criticalBattery->value());
    Common::savePowerSettings(CONF_SUSPEND_BATTERY_TIMEOUT,
                              autoSleepBattery->value());
    Common::savePowerSettings(CONF_SUSPEND_AC_TIMEOUT,
                              autoSleepAC->value());
    Common::savePowerSettings(CONF_FREEDESKTOP_SS,
                              desktopSS->isChecked());
    Common::savePowerSettings(CONF_FREEDESKTOP_PM,
                              desktopPM->isChecked());
    //Common::savePowerSettings(CONF_LID_XRANDR,
                              //lidXrandr->isChecked());
    Common::savePowerSettings(CONF_TRAY_NOTIFY,
                              showNotifications->isChecked());
    Common::savePowerSettings(CONF_TRAY_SHOW,
                              showSystemTray->isChecked());
    Common::savePowerSettings(CONF_LID_DISABLE_IF_EXTERNAL,
                              disableLidAction->isChecked());
    Common::savePowerSettings(CONF_SUSPEND_BATTERY_ACTION,
                              autoSleepBatteryAction->currentIndex());
    Common::savePowerSettings(CONF_SUSPEND_AC_ACTION,
                              autoSleepACAction->currentIndex());
    Common::savePowerSettings(CONF_BACKLIGHT_BATTERY_ENABLE,
                              backlightBatteryCheck->isChecked());
    Common::savePowerSettings(CONF_BACKLIGHT_AC_ENABLE,
                              backlightACCheck->isChecked());
    Common::savePowerSettings(CONF_BACKLIGHT_BATTERY,
                              backlightSliderBattery->value());
    Common::savePowerSettings(CONF_BACKLIGHT_AC,
                              backlightSliderAC->value());
    Common::savePowerSettings(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER,
                              backlightBatteryLowerCheck->isChecked());
    Common::savePowerSettings(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER,
                              backlightACHigherCheck->isChecked());
    Common::savePowerSettings(CONF_DIALOG,
                              saveGeometry());
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

// save current value and update power manager
void Dialog::handleLidActionBattery(int index)
{
    checkPerms();
    Common::savePowerSettings(CONF_LID_BATTERY_ACTION, index);
}

void Dialog::handleLidActionAC(int index)
{
    checkPerms();
    Common::savePowerSettings(CONF_LID_AC_ACTION, index);
}

void Dialog::handleCriticalAction(int index)
{
    checkPerms();
    Common::savePowerSettings(CONF_CRITICAL_BATTERY_ACTION, index);
}

void Dialog::handleCriticalBattery(int value)
{
    Common::savePowerSettings(CONF_CRITICAL_BATTERY_TIMEOUT, value);
}

void Dialog::handleAutoSleepBattery(int value)
{
    Common::savePowerSettings(CONF_SUSPEND_BATTERY_TIMEOUT, value);
 }

void Dialog::handleAutoSleepAC(int value)
{
    Common::savePowerSettings(CONF_SUSPEND_AC_TIMEOUT, value);
}

void Dialog::handleDesktopSS(bool triggered)
{
    Common::savePowerSettings(CONF_FREEDESKTOP_SS, triggered);
    QMessageBox::information(this, tr("Restart required"),
                             tr("You must restart the powerkit daemon to apply this setting"));
    // TODO: add restart now?
}

void Dialog::handleDesktopPM(bool triggered)
{
    Common::savePowerSettings(CONF_FREEDESKTOP_PM, triggered);
    QMessageBox::information(this, tr("Restart required"),
                             tr("You must restart the powerkit daemon to apply this setting"));
    // TODO: add restart now?
}

/*void Dialog::handleLidXrandr(bool triggered)
{
    Common::savePowerSettings(CONF_LID_XRANDR, triggered);
}*/

void Dialog::handleShowNotifications(bool triggered)
{
    Common::savePowerSettings(CONF_TRAY_NOTIFY, triggered);
}

void Dialog::handleShowSystemTray(bool triggered)
{
    Common::savePowerSettings(CONF_TRAY_SHOW, triggered);
}

void Dialog::handleDisableLidAction(bool triggered)
{
    /*if (triggered && !lidXrandr->isEnabled()) {
        lidXrandr->setEnabled(true);
    } else { lidXrandr->setDisabled(true); }*/
    Common::savePowerSettings(CONF_LID_DISABLE_IF_EXTERNAL, triggered);
}

void Dialog::handleAutoSleepBatteryAction(int index)
{
    checkPerms();
    Common::savePowerSettings(CONF_SUSPEND_BATTERY_ACTION, index);
}

void Dialog::handleAutoSleepACAction(int index)
{
    checkPerms();
    Common::savePowerSettings(CONF_SUSPEND_AC_ACTION, index);
}

void Dialog::handleLockscreenButton()
{
    man->LockScreen();
}

void Dialog::handleSleepButton()
{
    if (man->CanSuspend()) { man->Suspend(); }
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
    if (man->CanHibernate() &&
        Common::kernelCanResume()) { man->Hibernate(); }
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
    if (man->CanPowerOff()) { man->PowerOff(); }
    else {
        QMessageBox::information(this,
                                 tr("Power Action"),
                                 tr("System denied power request."
                                    " Maybe the required daemon is not running,"
                                    " or you may not have the required permissions."));
    }
}

void Dialog::checkPerms()
{
    if (!Common::kernelCanResume() || !hibernateButton->isEnabled()) {
        bool warnCantHibernate = false;
        if (criticalActionBattery->currentIndex() == criticalHibernate) {
            warnCantHibernate = true;
            criticalActionBattery->setCurrentIndex(criticalShutdown);
            handleCriticalAction(criticalShutdown);
        }
        if (lidActionAC->currentIndex() == lidHibernate ||
            lidActionAC->currentIndex() == lidHybridSleep) {
            warnCantHibernate = true;
            lidActionAC->setCurrentIndex(lidSleep);
            handleLidActionAC(lidSleep);
        }
        if (lidActionBattery->currentIndex() == lidHibernate ||
            lidActionBattery->currentIndex() == lidHybridSleep) {
            warnCantHibernate = true;
            lidActionBattery->setCurrentIndex(lidSleep);
            handleLidActionBattery(lidSleep);
        }
        if (autoSleepACAction->currentIndex() == suspendHibernate ||
            autoSleepACAction->currentIndex() == suspendHybrid) {
            warnCantHibernate = true;
            autoSleepACAction->setCurrentIndex(suspendSleep);
            handleAutoSleepACAction(suspendSleep);
        }
        if (autoSleepBatteryAction->currentIndex() == suspendHibernate ||
            autoSleepBatteryAction->currentIndex() == suspendHybrid) {
            warnCantHibernate = true;
            autoSleepBatteryAction->setCurrentIndex(suspendSleep);
            handleAutoSleepBatteryAction(suspendSleep);
        }
        if (warnCantHibernate) { hibernateWarn(); }
    }
    if (!sleepButton->isEnabled()) {
        bool warnCantSleep = false;
        if (lidActionAC->currentIndex() == lidSleep) {
            warnCantSleep = true;
            lidActionAC->setCurrentIndex(lidLock);
            handleLidActionAC(lidLock);
        }
        if (lidActionBattery->currentIndex() == lidSleep) {
            warnCantSleep = true;
            lidActionBattery->setCurrentIndex(lidLock);
            handleLidActionBattery(lidLock);
        }
        if (autoSleepACAction->currentIndex() == suspendSleep) {
            warnCantSleep = true;
            autoSleepACAction->setCurrentIndex(suspendNone);
            handleAutoSleepACAction(suspendNone);
        }
        if (autoSleepBatteryAction->currentIndex() == suspendSleep) {
            warnCantSleep = true;
            autoSleepBatteryAction->setCurrentIndex(suspendNone);
            handleAutoSleepBatteryAction(suspendNone);
        }
        if (warnCantSleep) { sleepWarn(); }
    }
}

void Dialog::handleBacklightSlider(int value)
{
    if (Common::backlightValue(backlightDevice) != value) {
        Common::adjustBacklight(backlightDevice, value);
    }
}

void Dialog::updateBacklight(QString file)
{
    Q_UNUSED(file);
    if (!hasBacklight) { return; }
    int value = Common::backlightValue(backlightDevice);
    if (value != backlightSlider->value()) {
        backlightSlider->setValue(value);
    }
}

void Dialog::checkDevices()
{
    double left = man->BatteryLeft();
    if (left<0) { left = 0; }
    if (left>100) { left = 100; }

    if (man->HasBattery()) {
        batteryLeftLCD->display(QDateTime::fromTime_t(man->OnBattery()?man->TimeToEmpty():man->TimeToFull())
                                .toUTC().toString("hh:mm"));
        batteryLabel->setText(QString("<h1 style=\"font-weight:normal;\">%1%</h1>").arg(left));
    } else {
        batteryLeftLCD->display("00:00");
        batteryLabel->setText(QString("<h1 style=\"font-weight:normal;\">%1</h1>").arg(tr("AC")));
    }

    QMapIterator<QString, Device*> i(man->getDevices());
    while (i.hasNext()) {
        i.next();
        //qDebug() << i.value()->name << i.value()->model << i.value()->type  << i.value()->isPresent << i.value()->objectName() << i.value()->percentage;
        QString uid = i.value()->path;
        if (!i.value()->isPresent) {
            if (deviceExists(uid)) { deviceRemove(uid); }
            continue;
        }
        if (!deviceExists(uid)) {
            QTreeWidgetItem *item = new QTreeWidgetItem(deviceTree);
            item->setText(0, i.value()->model.isEmpty()?i.value()->name:i.value()->model);
            item->setData(0, DEVICE_UUID, uid);
            item->setFlags(Qt::ItemIsEnabled);
            QIcon itemIcon;
            switch(i.value()->type) {
            case Device::DeviceKeyboard:
                itemIcon = QIcon::fromTheme(DEFAULT_KEYBOARD_ICON);
                break;
            case Device::DeviceMouse:
                itemIcon = QIcon::fromTheme(DEFAULT_MOUSE_ICON);
                break;
            default:
                itemIcon = QIcon::fromTheme(DEFAULT_BATTERY_ICON);
            }
            item->setIcon(0, itemIcon);
            devicesProg[uid] = new QProgressBar(this);
            devicesProg[uid]->setMinimum(0);
            devicesProg[uid]->setMaximum(100);
            devicesProg[uid]->setValue(i.value()->percentage);
            deviceTree->setItemWidget(item, 1, devicesProg[uid]);
        } else {
            devicesProg[i.value()->path]->setValue(i.value()->percentage);
        }
    }

    QIcon icon = QIcon::fromTheme(DEFAULT_AC_ICON);
    if (left == 0 || !man->HasBattery()) {
        batteryIcon->setPixmap(icon.pixmap(QSize(48, 48)));
        return;
    }
    if (left <= 10) {
        icon = QIcon::fromTheme(man->OnBattery()?DEFAULT_BATTERY_ICON_CRIT:DEFAULT_BATTERY_ICON_CRIT_AC);
    } else if (left <= 25) {
        icon = QIcon::fromTheme(man->OnBattery()?DEFAULT_BATTERY_ICON_LOW:DEFAULT_BATTERY_ICON_LOW_AC);
    } else if (left <= 75) {
        icon = QIcon::fromTheme(man->OnBattery()?DEFAULT_BATTERY_ICON_GOOD:DEFAULT_BATTERY_ICON_GOOD_AC);
    } else if (left <= 90) {
        icon = QIcon::fromTheme(man->OnBattery()?DEFAULT_BATTERY_ICON_FULL:DEFAULT_BATTERY_ICON_FULL_AC);
    } else {
        icon = QIcon::fromTheme(man->OnBattery()?DEFAULT_BATTERY_ICON_FULL:DEFAULT_BATTERY_ICON_CHARGED);
        if (left == 100 && !man->OnBattery()) {
            icon = QIcon::fromTheme(DEFAULT_AC_ICON);
        }
    }
    batteryIcon->setPixmap(icon.pixmap(QSize(48, 48)));
}

bool Dialog::deviceExists(QString uid)
{
    for (int i=0;i<deviceTree->topLevelItemCount();++i) {
        QTreeWidgetItem *item = deviceTree->topLevelItem(i);
        if (!item) { continue; }
        if (item->data(0, DEVICE_UUID) == uid) { return true; }
    }
    return false;
}

void Dialog::deviceRemove(QString uid)
{
    for (int i=0;i<deviceTree->topLevelItemCount();++i) {
        QTreeWidgetItem *item = deviceTree->topLevelItem(i);
        if (!item) { continue; }
        if (item->data(0, DEVICE_UUID) == uid) {
            delete deviceTree->takeTopLevelItem(i);
        }
    }
    if (devicesProg.contains(uid)) {
        devicesProg[uid]->deleteLater();
        devicesProg.remove(uid);
    }
}

void Dialog::handleDeviceAdded(QString uid)
{
    Q_UNUSED(uid)
    checkDevices();
}

void Dialog::handleBacklightBatteryCheck(bool triggered)
{
    Common::savePowerSettings(CONF_BACKLIGHT_BATTERY_ENABLE, triggered);
    handleBacklightBatterySlider(backlightSliderBattery->value());
}

void Dialog::handleBacklightACCheck(bool triggered)
{
    Common::savePowerSettings(CONF_BACKLIGHT_AC_ENABLE, triggered);
    handleBacklightACSlider(backlightSliderAC->value());
}

void Dialog::handleBacklightBatterySlider(int value)
{
    Common::savePowerSettings(CONF_BACKLIGHT_BATTERY, value);
}

void Dialog::handleBacklightACSlider(int value)
{
    Common::savePowerSettings(CONF_BACKLIGHT_AC, value);
}

void Dialog::hibernateWarn()
{
    QMessageBox::warning(this, tr("Hibernate not supported"),
                         tr("The kernel command line does not contain resume=<swap partition/file>."
                            "Add resume=<swap partition/file> to the boot loader configuration"
                            " (GRUB/LILO etc) to enable hibernate."));
}

void Dialog::sleepWarn()
{
    QMessageBox::warning(this, tr("Sleep not supported"),
                         tr("Sleep not supported, consult your OS documentation."));
}

void Dialog::handleBacklightBatteryCheckLower(bool triggered)
{
    Common::savePowerSettings(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER, triggered);
}

void Dialog::handleBacklightACCheckHigher(bool triggered)
{
    Common::savePowerSettings(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER, triggered);
}

void Dialog::handleUpdatedInhibitors()
{
    getInhibitors();
}

void Dialog::getInhibitors()
{
    if (!dbus->isValid()) { return; }
    inhibitorTree->clear();
    QDBusMessage replySS =  dbus->call("ScreenSaverInhibitors");
    QDBusMessage replyPM =  dbus->call("PowerManagementInhibitors");
    QStringList ssList = replySS.arguments().last().toStringList();
    QStringList pmList = replyPM.arguments().last().toStringList();
    for (int i=0;i<ssList.size();++i) {
        QString inhibitor = ssList.at(i);
        if (inhibitor.isEmpty()) { continue; }
        QTreeWidgetItem *item = new QTreeWidgetItem(inhibitorTree);
        item->setText(0, inhibitor);
        item->setFlags(Qt::ItemIsEnabled);
        item->setIcon(0, QIcon::fromTheme("application-x-executable"));
    }
    for (int i=0;i<pmList.size();++i) {
        QString inhibitor = pmList.at(i);
        if (inhibitor.isEmpty()) { continue; }
        QTreeWidgetItem *item = new QTreeWidgetItem(inhibitorTree);
        item->setText(0, inhibitor);
        item->setFlags(Qt::ItemIsEnabled);
        item->setIcon(0, QIcon::fromTheme("application-x-executable"));
    }
}

void Dialog::enableBacklight(bool enabled)
{
    backlightSlider->setEnabled(enabled);
    backlightSliderBattery->setEnabled(enabled);
    backlightSliderAC->setEnabled(enabled);
    backlightBatteryCheck->setEnabled(enabled);
    backlightACCheck->setEnabled(enabled);
    backlightBatteryLowerCheck->setEnabled(enabled);
    backlightACHigherCheck->setEnabled(enabled);
}
