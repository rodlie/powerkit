/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "dialog.h"
#include "login1.h"
#include "ckit.h"

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
{
    // setup dialog
    setAttribute(Qt::WA_QuitOnClose, true);
    setWindowTitle(tr("Power Manager"));
    setWindowIcon(QIcon::fromTheme(DEFAULT_BATTERY_ICON));
    setMinimumSize(QSize(380, 320));

    // setup dbus
    QDBusConnection session = QDBusConnection::sessionBus();
    dbus = new QDBusInterface(PD_SERVICE, PD_PATH, PD_SERVICE,
                              session, this);

    // setup theme
    Common::setIconTheme();

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

    QWidget *batteryContainer = new QWidget(this);
    QVBoxLayout *batteryContainerLayout = new QVBoxLayout(batteryContainer);
    batteryContainerLayout->setMargin(0);
    batteryContainerLayout->setSpacing(0);

    QWidget *lidActionBatteryContainer = new QWidget(this);
    QHBoxLayout *lidActionBatteryContainerLayout = new QHBoxLayout(lidActionBatteryContainer);
    lidActionBattery = new QComboBox(this);
    QLabel *lidActionBatteryLabel = new QLabel(this);

    disableLidAction = new QCheckBox(this);
    disableLidAction->setIcon(QIcon::fromTheme(DEFAULT_VIDEO_ICON));
    disableLidAction->setText(tr("Disable lid action if external monitor(s) is connected"));

    QLabel *lidActionBatteryIcon = new QLabel(this);
    lidActionBatteryIcon->setMaximumSize(48, 48);
    lidActionBatteryIcon->setMinimumSize(48, 48);
    lidActionBatteryIcon->setPixmap(QIcon::fromTheme(DEFAULT_VIDEO_ICON).pixmap(QSize(48, 48)));
    lidActionBatteryLabel->setText(tr("<h3 style=\"font-weight:normal;\">Lid action</h3>"));
    lidActionBatteryContainerLayout->addWidget(lidActionBatteryIcon);
    lidActionBatteryContainerLayout->addWidget(lidActionBatteryLabel);
    lidActionBatteryContainerLayout->addWidget(lidActionBattery);
    batteryContainerLayout->addWidget(lidActionBatteryContainer);

    QWidget *criticalBatteryContainer = new QWidget(this);
    QHBoxLayout *criticalBatteryContainerLayout = new QHBoxLayout(criticalBatteryContainer);
    criticalBattery = new QSpinBox(this);
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
    criticalActionBatteryContainerLayout->addWidget(criticalBattery);
    criticalActionBatteryContainerLayout->addWidget(criticalActionBattery);

    QLabel *criticalBatteryIcon = new QLabel(this);
    criticalBatteryIcon->setMaximumSize(48, 48);
    criticalBatteryIcon->setMinimumSize(48, 48);
    criticalBatteryIcon->setPixmap(QIcon::fromTheme(DEFAULT_BATTERY_ICON_CRIT).pixmap(QSize(48, 48)));
    criticalBatteryLabel->setText(tr("<h3 style=\"font-weight:normal;\">Critical battery</h3>"));
    criticalBatteryContainerLayout->addWidget(criticalBatteryIcon);
    criticalBatteryContainerLayout->addWidget(criticalBatteryLabel);
    criticalBatteryContainerLayout->addWidget(criticalActionBatteryContainer);

    QWidget *sleepBatteryContainer = new QWidget(this);
    QHBoxLayout *sleepBatteryContainerLayout = new QHBoxLayout(sleepBatteryContainer);
    autoSleepBattery = new QSpinBox(this);
    autoSleepBattery->setMinimum(0);
    autoSleepBattery->setMaximum(1000);
    autoSleepBattery->setSuffix(tr(" min"));
    QLabel *sleepBatteryLabel = new QLabel(this);

    QWidget *sleepActionBatteryContainer = new QWidget(this);
    sleepActionBatteryContainer->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *sleepActionBatteryContainerLayout = new QVBoxLayout(sleepActionBatteryContainer);
    sleepActionBatteryContainerLayout->setMargin(0);
    sleepActionBatteryContainerLayout->setSpacing(0);
    autoSleepBatteryAction = new QComboBox(this);
    sleepActionBatteryContainerLayout->addWidget(autoSleepBattery);
    sleepActionBatteryContainerLayout->addWidget(autoSleepBatteryAction);

    QLabel *sleepBatteryIcon = new QLabel(this);
    sleepBatteryIcon->setMaximumSize(48, 48);
    sleepBatteryIcon->setMinimumSize(48, 48);
    sleepBatteryIcon->setPixmap(QIcon::fromTheme(DEFAULT_SUSPEND_ICON).pixmap(QSize(48, 48)));
    sleepBatteryLabel->setText(tr("<h3 style=\"font-weight:normal;\">Suspend after</h3>"));
    sleepBatteryContainerLayout->addWidget(sleepBatteryIcon);
    sleepBatteryContainerLayout->addWidget(sleepBatteryLabel);
    sleepBatteryContainerLayout->addWidget(sleepActionBatteryContainer);

    batteryContainerLayout->addWidget(sleepBatteryContainer);
    batteryContainerLayout->addWidget(criticalBatteryContainer);

    batteryContainerLayout->addStretch();

    QWidget *acContainer = new QWidget(this);
    QVBoxLayout *acContainerLayout = new QVBoxLayout(acContainer);
    acContainerLayout->setMargin(0);
    acContainerLayout->setSpacing(0);

    QWidget *lidActionACContainer = new QWidget(this);
    QHBoxLayout *lidActionACContainerLayout = new QHBoxLayout(lidActionACContainer);
    lidActionAC = new QComboBox(this);
    QLabel *lidActionACLabel = new QLabel(this);


    QLabel *lidActionACIcon = new QLabel(this);
    lidActionACIcon->setMaximumSize(48, 48);
    lidActionACIcon->setMinimumSize(48, 48);
    lidActionACIcon->setPixmap(QIcon::fromTheme(DEFAULT_VIDEO_ICON).pixmap(QSize(48, 48)));
    lidActionACLabel->setText(tr("<h3 style=\"font-weight:normal;\">Lid action</h3>"));
    lidActionACContainerLayout->addWidget(lidActionACIcon);
    lidActionACContainerLayout->addWidget(lidActionACLabel);
    lidActionACContainerLayout->addWidget(lidActionAC);
    acContainerLayout->addWidget(lidActionACContainer);

    QWidget *sleepACContainer = new QWidget(this);
    QHBoxLayout *sleepACContainerLayout = new QHBoxLayout(sleepACContainer);
    autoSleepAC = new QSpinBox(this);
    autoSleepAC->setMinimum(0);
    autoSleepAC->setMaximum(1000);
    autoSleepAC->setSuffix(tr(" min"));
    QLabel *sleepACLabel = new QLabel(this);

    sleepACContainerLayout->addWidget(sleepACLabel);
    acContainerLayout->addWidget(sleepACContainer);

    QWidget *sleepActionACContainer = new QWidget(this);
    sleepActionACContainer->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *sleepActionACContainerLayout = new QVBoxLayout(sleepActionACContainer);
    sleepActionACContainerLayout->setMargin(0);
    sleepActionACContainerLayout->setSpacing(0);
    autoSleepACAction = new QComboBox(this);
    sleepActionACContainerLayout->addWidget(autoSleepAC);
    sleepActionACContainerLayout->addWidget(autoSleepACAction);

    QLabel *sleepACIcon = new QLabel(this);
    sleepACIcon->setMaximumSize(48,48);
    sleepACIcon->setMinimumSize(48,48);
    sleepACIcon->setPixmap(QIcon::fromTheme(DEFAULT_SUSPEND_ICON).pixmap(QSize(48, 48)));
    sleepACLabel->setText(tr("<h3 style=\"font-weight:normal;\">Suspend after</h3>"));
    sleepACContainerLayout->addWidget(sleepACIcon);
    sleepACContainerLayout->addWidget(sleepACLabel);
    sleepACContainerLayout->addWidget(sleepActionACContainer);
    acContainerLayout->addWidget(sleepACContainer);

    acContainerLayout->addStretch();

    QWidget *advContainer = new QWidget(this);
    QVBoxLayout *advContainerLayout = new QVBoxLayout(advContainer);

    showSystemTray  = new QCheckBox(this);
    showSystemTray->setIcon(QIcon::fromTheme(DEFAULT_TRAY_ICON));
    showSystemTray->setText(tr("Show system tray"));

    showNotifications = new QCheckBox(this);
    showNotifications->setIcon(QIcon::fromTheme(DEFAULT_NOTIFY_ICON));
    showNotifications->setText(tr("Show notifications"));

    desktopSS = new QCheckBox(this);
    desktopSS->setIcon(QIcon::fromTheme(DEFAULT_VIDEO_ICON));
    desktopSS->setText("org.freedesktop.ScreenSaver");

    desktopPM = new QCheckBox(this);
    desktopPM->setIcon(QIcon::fromTheme(DEFAULT_BATTERY_ICON));
    desktopPM->setText("org.freedesktop.PowerManagement");

    advContainerLayout->addWidget(showSystemTray);
    advContainerLayout->addWidget(showNotifications);
    advContainerLayout->addWidget(disableLidAction);
    advContainerLayout->addWidget(desktopSS);
    advContainerLayout->addWidget(desktopPM);
    advContainerLayout->addStretch();

    QWidget *extraContainer = new QWidget(this);
    QHBoxLayout *extraContainerLayout = new QHBoxLayout(extraContainer);

    QLabel *powerLabel = new QLabel(this);
    powerLabel->setText(QString("<a href=\"https://github.com/rodlie/powerdwarf\">"
                           "powerdwarf</a> version %1<br>&copy; 2018 Ole-André Rodlie").arg(QApplication::applicationVersion()));

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

    extraContainerLayout->addWidget(powerLabel);
    extraContainerLayout->addStretch();
    extraContainerLayout->addWidget(lockscreenButton);
    extraContainerLayout->addWidget(sleepButton);
    extraContainerLayout->addWidget(hibernateButton);
    extraContainerLayout->addWidget(poweroffButton);

    layout->addWidget(wrapper);
    layout->addWidget(extraContainer);

    containerWidget->addTab(batteryContainer,
                            QIcon::fromTheme(DEFAULT_BATTERY_ICON),
                            tr("Battery"));
    containerWidget->addTab(acContainer,
                            QIcon::fromTheme(DEFAULT_AC_ICON),
                            tr("AC"));
    containerWidget->addTab(advContainer,
                            QIcon::fromTheme(DEFAULT_TRAY_ICON),
                            tr("Advanced"));

    populate(); // populate boxes
    loadSettings(); // load settings

    // connect various widgets
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
}

Dialog::~Dialog()
{
    Common::savePowerSettings("dialog_geometry", saveGeometry());
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

    lidActionAC->clear();
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_NONE_ICON),
                         tr("None"), lidNone);
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_LOCK_ICON),
                         tr("Lock Screen"), lidLock);
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                         tr("Sleep"), lidSleep);
    lidActionAC->addItem(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON),
                         tr("Hibernate"), lidHibernate);

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

    autoSleepACAction->clear();
    autoSleepACAction->addItem(QIcon::fromTheme(DEFAULT_NONE_ICON),
                               tr("None"), suspendNone);
    autoSleepACAction->addItem(QIcon::fromTheme(DEFAULT_SUSPEND_ICON),
                               tr("Sleep"), suspendSleep);
    autoSleepACAction->addItem(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON),
                               tr("Hibernate"), suspendHibernate);
    autoSleepACAction->addItem(QIcon::fromTheme(DEFAULT_SHUTDOWN_ICON),
                               tr("Shutdown"), suspendShutdown);
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

    if (Login1::hasService()) {
        sleepButton->setEnabled(Login1::canSuspend());
        hibernateButton->setEnabled(Login1::canHibernate());
        poweroffButton->setEnabled(Login1::canPowerOff());
    } else {
        sleepButton->setEnabled(UPower::canSuspend());
        hibernateButton->setEnabled(UPower::canHibernate());
        poweroffButton->setEnabled(CKit::canPowerOff());
    }

    checkPerms();
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
    if (index == lidHibernate &&
            (!Common::kernelCanResume() ||
             !hibernateButton->isEnabled())) {
        QMessageBox::warning(this, tr("Error"), tr("Hibernate is not supported"));
        return;
    } else if (index == lidSleep &&
               !sleepButton->isEnabled()) {
        QMessageBox::warning(this, tr("Error"), tr("Suspend is not supported"));
        return;
    }
    Common::savePowerSettings(CONF_LID_BATTERY_ACTION, index);
}

void Dialog::handleLidActionAC(int index)
{
    if (index == lidHibernate &&
            (!Common::kernelCanResume() ||
             !hibernateButton->isEnabled())) {
        QMessageBox::warning(this, tr("Error"), tr("Hibernate is not supported"));
        return;
    } else if (index == lidSleep &&
               !sleepButton->isEnabled()) {
        QMessageBox::warning(this, tr("Error"), tr("Suspend is not supported"));
        return;
    }
    Common::savePowerSettings(CONF_LID_AC_ACTION, index);
}

void Dialog::handleCriticalAction(int index)
{
    if (index == criticalHibernate &&
            (!Common::kernelCanResume() ||
             !hibernateButton->isEnabled())) {
        QMessageBox::warning(this, tr("Error"), tr("Hibernate is not supported"));
        return;
    }
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
                             tr("You must restart the power daemon to apply this setting"));
    // TODO: add restart now?
}

void Dialog::handleDesktopPM(bool triggered)
{
    Common::savePowerSettings(CONF_FREEDESKTOP_PM, triggered);
    QMessageBox::information(this, tr("Restart required"),
                             tr("You must restart the power daemon to apply this setting"));
    // TODO: add restart now?
}

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
    Common::savePowerSettings(CONF_LID_DISABLE_IF_EXTERNAL, triggered);
}

void Dialog::handleAutoSleepBatteryAction(int index)
{
    if (index == suspendHibernate &&
            (!Common::kernelCanResume() ||
             !hibernateButton->isEnabled())) {
        QMessageBox::warning(this, tr("Error"), tr("Hibernate is not supported"));
        return;
    } else if (index == suspendSleep &&
               !sleepButton->isEnabled()) {
        QMessageBox::warning(this, tr("Error"), tr("Suspend is not supported"));
        return;
    }
    Common::savePowerSettings(CONF_SUSPEND_BATTERY_ACTION, index);
}

void Dialog::handleAutoSleepACAction(int index)
{
    if (index == suspendHibernate &&
            (!Common::kernelCanResume() ||
             !hibernateButton->isEnabled())) {
        QMessageBox::warning(this, tr("Error"), tr("Hibernate is not supported"));
        return;
    } else if (index == suspendSleep &&
               !sleepButton->isEnabled()) {
        QMessageBox::warning(this, tr("Error"), tr("Suspend is not supported"));
        return;
    }
    Common::savePowerSettings(CONF_SUSPEND_AC_ACTION, index);
}

void Dialog::handleLockscreenButton()
{
    QProcess proc;
    proc.start(XSCREENSAVER_LOCK);
    proc.waitForFinished();
    proc.close();
}

void Dialog::handleSleepButton()
{
    if (Login1::hasService()) {
        if (Login1::canSuspend()) { Login1::suspend(); }
    } else if (UPower::canSuspend()) { UPower::suspend(); }
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
    if (Login1::hasService()) {
        if (Login1::canHibernate()) { Login1::hibernate(); }
    } else if (UPower::canHibernate()) { UPower::hibernate(); }
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
    if (Login1::hasService()) {
        if (Login1::canPowerOff()) { Login1::poweroff(); }
    } else if (CKit::hasService()) {
        if (CKit::canPowerOff()) { CKit::poweroff(); }
    } else {
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
        QMessageBox::warning(this, tr("Hibernate not supported"),
                             tr("The kernel command line does not contain resume=<swap partition/file>."
                                "Add resume=<swap partition/file> to the boot loader configuration"
                                " (GRUB/LILO etc) to enable hibernate."));
        hibernateButton->setDisabled(true);
        if (criticalActionBattery->currentIndex() == criticalHibernate) {
            criticalActionBattery->setCurrentIndex(criticalShutdown);
            handleCriticalAction(criticalShutdown);
        }
        if (lidActionAC->currentIndex() == lidHibernate) {
            lidActionAC->setCurrentIndex(lidLock);
            handleLidActionAC(lidLock);
        }
        if (lidActionBattery->currentIndex() == lidHibernate) {
            lidActionBattery->setCurrentIndex(lidLock);
            handleLidActionBattery(lidLock);
        }
        if (autoSleepACAction->currentIndex() == suspendHibernate) {
            autoSleepACAction->setCurrentIndex(suspendNone);
            handleAutoSleepACAction(suspendNone);
        }
        if (autoSleepBatteryAction->currentIndex() == suspendHibernate) {
            autoSleepBatteryAction->setCurrentIndex(suspendNone);
            handleAutoSleepBatteryAction(suspendNone);
        }
    }
    if (!sleepButton->isEnabled()) {
        if (lidActionAC->currentIndex() == lidSleep) {
            lidActionAC->setCurrentIndex(lidLock);
            handleLidActionAC(lidLock);
        }
        if (lidActionBattery->currentIndex() == lidSleep) {
            lidActionBattery->setCurrentIndex(lidLock);
            handleLidActionBattery(lidLock);
        }
        if (autoSleepACAction->currentIndex() == suspendSleep) {
            autoSleepACAction->setCurrentIndex(suspendNone);
            handleAutoSleepACAction(suspendNone);
        }
        if (autoSleepBatteryAction->currentIndex() == suspendSleep) {
            autoSleepBatteryAction->setCurrentIndex(suspendNone);
            handleAutoSleepBatteryAction(suspendNone);
        }
    }
}
