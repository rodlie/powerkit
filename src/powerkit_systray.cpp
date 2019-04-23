/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit_systray.h"
#include "powerkit_def.h"
#include "powerkit_theme.h"
#include "powerkit_settings.h"
#include "powerkit_backlight.h"

#include <QMessageBox>
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>

SysTray::SysTray(QObject *parent)
    : QObject(parent)
    , tray(nullptr)
    , man(nullptr)
    , pm(nullptr)
    , ss(nullptr)
    , wasLowBattery(false)
    , wasVeryLowBattery(false)
    , lowBatteryValue(LOW_BATTERY)
    , critBatteryValue(CRITICAL_BATTERY)
    , hasService(false)
    , lidActionBattery(LID_BATTERY_DEFAULT)
    , lidActionAC(LID_AC_DEFAULT)
    , criticalAction(CRITICAL_DEFAULT)
    , autoSuspendBattery(AUTO_SLEEP_BATTERY)
    , autoSuspendAC(0)
    , timer(nullptr)
    , timeouts(0)
    , showNotifications(true)
    , desktopSS(true)
    , desktopPM(true)
    , showTray(true)
    , disableLidOnExternalMonitors(false)
    , autoSuspendBatteryAction(suspendSleep)
    , autoSuspendACAction(suspendNone)
    , xscreensaver(nullptr)
    , startupScreensaver(true)
    , watcher(nullptr)
    , lidXrandr(false)
    , lidWasClosed(false)
    , hasBacklight(false)
    , backlightOnBattery(false)
    , backlightOnAC(false)
    , backlightBatteryValue(0)
    , backlightACValue(0)
    , backlightBatteryDisableIfLower(false)
    , backlightACDisableIfHigher(false)
    , configDialog(nullptr)
    , warnOnLowBattery(true)
    , warnOnVeryLowBattery(true)
    , notifyOnBattery(true)
    , notifyOnAC(true)
    , backlightMouseWheel(true)
    , ignoreKernelResume(false)
    , powerMenu(nullptr)
    , actSettings(nullptr)
    , actPowerOff(nullptr)
    , actRestart(nullptr)
    , actSuspend(nullptr)
    , actHibernate(nullptr)
    , labelBatteryStatus(nullptr)
    , labelBatteryIcon(nullptr)
    //, labelBatteryLeft(0)
    , menuFrame(nullptr)
    , menuHeader(nullptr)
    , backlightSlider(nullptr)
    , backlightLabel(nullptr)
    , backlightWatcher(nullptr)
    , deviceTree(nullptr)
    , inhibitorTree(nullptr)
    , powerTab(nullptr)
{
    // setup tray
    tray = new TrayIcon(this);
    connect(tray,
            SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,
            SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
    connect(tray,
            SIGNAL(wheel(TrayIcon::WheelAction)),
            this,
            SLOT(handleTrayWheel(TrayIcon::WheelAction)));

    // setup manager
    man = new PowerKit(this);
    connect(man,
            SIGNAL(UpdatedDevices()),
            this,
            SLOT(checkDevices()));
    connect(man,
            SIGNAL(LidClosed()),
            this,
            SLOT(handleClosedLid()));
    connect(man,
            SIGNAL(LidOpened()),
            this,
            SLOT(handleOpenedLid()));
    connect(man,
            SIGNAL(SwitchedToBattery()),
            this,
            SLOT(handleOnBattery()));
    connect(man,
            SIGNAL(SwitchedToAC()),
            this,
            SLOT(handleOnAC()));
    connect(man,
            SIGNAL(PrepareForSuspend()),
            this,
            SLOT(handlePrepareForSuspend()));
    connect(man,
            SIGNAL(PrepareForResume()),
            this,
            SLOT(handlePrepareForResume()));
    connect(man,
            SIGNAL(DeviceWasAdded(QString)),
            this,
            SLOT(handleDeviceChanged(QString)));
    connect(man,
            SIGNAL(DeviceWasRemoved(QString)),
            this,
            SLOT(handleDeviceChanged(QString)));
    connect(man,
            SIGNAL(Update()),
            this,
            SLOT(loadSettings()));
    connect(man,
            SIGNAL(UpdatedInhibitors()),
            this,
            SLOT(getInhibitors()));

    // setup org.freedesktop.PowerManagement
    pm = new PowerManagement(this);
    connect(pm,
            SIGNAL(HasInhibitChanged(bool)),
            this,
            SLOT(handleHasInhibitChanged(bool)));
    connect(pm,
            SIGNAL(newInhibit(QString,QString,quint32)),
            this,
            SLOT(handleNewInhibitPowerManagement(QString,QString,quint32)));
    connect(pm,
            SIGNAL(removedInhibit(quint32)),
            this,
            SLOT(handleDelInhibitPowerManagement(quint32)));
    connect(pm,
            SIGNAL(newInhibit(QString,QString,quint32)),
            man,
            SLOT(handleNewInhibitPowerManagement(QString,QString,quint32)));
    connect(pm,
            SIGNAL(removedInhibit(quint32)),
            man,
            SLOT(handleDelInhibitPowerManagement(quint32)));

    // setup org.freedesktop.ScreenSaver
    ss = new ScreenSaver(this);
    connect(ss,
            SIGNAL(newInhibit(QString,QString,quint32)),
            this,
            SLOT(handleNewInhibitScreenSaver(QString,QString,quint32)));
    connect(ss,
            SIGNAL(removedInhibit(quint32)),
            this,
            SLOT(handleDelInhibitScreenSaver(quint32)));
    connect(ss,
            SIGNAL(newInhibit(QString,QString,quint32)),
            man,
            SLOT(handleNewInhibitScreenSaver(QString,QString,quint32)));
    connect(ss,
            SIGNAL(removedInhibit(quint32)),
            man,
            SLOT(handleDelInhibitScreenSaver(quint32)));

    // setup xscreensaver
    xscreensaver = new QProcess(this);
    connect(xscreensaver,
            SIGNAL(finished(int)),
            this,
            SLOT(handleScreensaverFinished(int)));

    // setup timer
    timer = new QTimer(this);
    timer->setInterval(60000);
    connect(timer,
            SIGNAL(timeout()),
            this,
            SLOT(timeout()));
    timer->start();

    // check for config
    PowerSettings::getConf();

    // setup theme
    Theme::setIconTheme();
    if (tray->icon().isNull()) {
        tray->setIcon(QIcon::fromTheme(DEFAULT_BATTERY_ICON));
    }

    // load settings and register service
    loadSettings();
    registerService();

    // start xscreensaver
    if (desktopSS) {
        xscreensaver->start(XSCREENSAVER_RUN);
    }

    // setup backlight
    backlightWatcher = new QFileSystemWatcher(this);
    backlightWatcher->addPath(QString("%1/brightness").arg(backlightDevice));
    connect(backlightWatcher, SIGNAL(fileChanged(QString)),
            this, SLOT(updateBacklight(QString)));

    // device check
    QTimer::singleShot(10000,
                       this,
                       SLOT(checkDevices()));
    QTimer::singleShot(1000,
                       this,
                       SLOT(setInternalMonitor()));

    // menu
    powerMenu  = new QMenu(NULL);
    tray->setContextMenu(powerMenu);
    /*QTimer::singleShot(1000,
                       this,
                       SLOT(*/populateMenu()/*))*/;

    // config dialog
    configDialog = new QProcess(this);
    connect(configDialog,
            SIGNAL(finished(int)),
            this,
            SLOT(handleConfigDialogFinished(int)));

    // setup watcher
    watcher = new QFileSystemWatcher(this);
    watcher->addPath(PowerSettings::getDir());
    watcher->addPath(PowerSettings::getConf());
    connect(watcher,
            SIGNAL(fileChanged(QString)),
            this,
            SLOT(handleConfChanged(QString)));
    connect(watcher,
            SIGNAL(directoryChanged(QString)),
            this,
            SLOT(handleConfChanged(QString)));
}

SysTray::~SysTray()
{
    menuFrame->deleteLater();
    menuHeader->deleteLater();
    powerMenu->deleteLater();
    if (xscreensaver->isOpen()) { xscreensaver->close(); }
}

// what to do when user clicks systray
void SysTray::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::Context:
    case QSystemTrayIcon::DoubleClick:
    case QSystemTrayIcon::MiddleClick:
        powerMenu->exec(QCursor::pos());
        break;
    default:;
    }
}

void SysTray::checkDevices()
{
    // show/hide tray
    if (tray->isSystemTrayAvailable() &&
        !tray->isVisible() &&
        showTray) { tray->show(); }
    if (!showTray &&
        tray->isVisible()) { tray->hide(); }

    // update menu items
    updateMenu();

    // update power devices
    updatePowerDevices();

    // get battery left and add tooltip
    double batteryLeft = man->BatteryLeft();
    qDebug() << "battery at" << batteryLeft;
    if (batteryLeft > 0 && man->HasBattery()) {
        tray->setToolTip(QString("%1 %2%").arg(tr("Battery at")).arg(batteryLeft));
        if (man->TimeToEmpty()>0 && man->OnBattery()) {
            tray->setToolTip(tray->toolTip()
                             .append(QString(", %1 %2")
                             .arg(QDateTime::fromTime_t((uint)man->TimeToEmpty())
                                                        .toUTC().toString("hh:mm")))
                             .arg(tr("left")));
        }
        if (batteryLeft > 99) { tray->setToolTip(tr("Charged")); }
        if (!man->OnBattery() &&
            man->BatteryLeft() <= 99) {
            if (man->TimeToFull()>0) {
                tray->setToolTip(tray->toolTip()
                                 .append(QString(", %1 %2")
                                 .arg(QDateTime::fromTime_t((uint)man->TimeToFull())
                                                            .toUTC().toString("hh:mm")))
                                 .arg(tr("left")));
            }
            tray->setToolTip(tray->toolTip().append(QString(" (%1)").arg(tr("Charging"))));
        }
    } else { tray->setToolTip(tr("On AC")); }

    // inhibitors tooltip
    /*if (ssInhibitors.size()>0) {
        QString tooltip = "\n\n";
        tooltip.append(QString("%1:\n").arg(tr("Screen Saver Inhibitors")));
        QMapIterator<quint32, QString> i(ssInhibitors);
        while (i.hasNext()) {
            i.next();
            tooltip.append(QString(" * %1\n").arg(i.value()));
        }
        tray->setToolTip(tray->toolTip().append(tooltip));
    }
    if (pmInhibitors.size()>0) {
        QString tooltip = "\n\n";
        tooltip.append(QString("%1:\n").arg(tr("Power Manager Inhibitors")));
        QMapIterator<quint32, QString> i(pmInhibitors);
        while (i.hasNext()) {
            i.next();
            tooltip.append(QString(" * %1\n").arg(i.value()));
        }
        tray->setToolTip(tray->toolTip().append(tooltip));
    }*/

    // draw battery systray
    drawBattery(batteryLeft);

    // low battery?
    handleLow(batteryLeft);

    // very low battery?
    handleVeryLow(batteryLeft);

    // critical battery?
    handleCritical(batteryLeft);

    // Register service if not already registered
    if (!hasService) { registerService(); }
}

// what to do when user close lid
void SysTray::handleClosedLid()
{
    qDebug() << "lid closed";
    lidWasClosed = true;

    int type = lidNone;
    if (man->OnBattery()) {  // on battery
        type = lidActionBattery;
    } else { // on ac
        type = lidActionAC;
    }

    if (disableLidOnExternalMonitors &&
            externalMonitorIsConnected()) {
        qDebug() << "external monitor is connected, ignore lid action";
        switchInternalMonitor(false /* turn off screen */);
        return;
    }

    qDebug() << "lid action" << type;
    switch(type) {
    case lidLock:
        man->LockScreen();
        break;
    case lidSleep:
        man->Suspend();
        break;
    case lidHibernate:
        man->Hibernate();
        break;
    case lidShutdown:
        man->PowerOff();
        break;
    case lidHybridSleep:
        man->HybridSleep();
        break;
    default: ;
    }
}

// what to do when user open lid
void SysTray::handleOpenedLid()
{
    qDebug() << "lid is now open";
    lidWasClosed = false;
    if (disableLidOnExternalMonitors) {
        switchInternalMonitor(true /* turn on screen */);
    }
}

// do something when switched to battery power
void SysTray::handleOnBattery()
{
    if (notifyOnBattery) {
        showMessage(tr("On Battery"),
                    tr("Switched to battery power."));
    }

    // brightness
    if (/*hasBacklight &&*/
        backlightOnBattery &&
        backlightBatteryValue>0) {
        qDebug() << "set brightness on battery";
        if (backlightBatteryDisableIfLower &&
            backlightBatteryValue>PowerBacklight::getCurrentBrightness(backlightDevice)) {
            qDebug() << "brightness is lower than battery value, ignore";
            return;
        }
        /*if (hasBacklight) {
            Common::adjustBacklight(backlightDevice, backlightBatteryValue);
        } else {*/
            man->setDisplayBacklight(backlightDevice, backlightBatteryValue);
        //}
    }
}

// do something when switched to ac power
void SysTray::handleOnAC()
{
    if (notifyOnAC) {
        showMessage(tr("On AC"),
                    tr("Switched to AC power."));
    }

    wasLowBattery = false;
    wasVeryLowBattery = false;

    // brightness
    if (/*hasBacklight &&*/
        backlightOnAC &&
        backlightACValue>0) {
        qDebug() << "set brightness on ac";
        if (backlightACDisableIfHigher &&
            backlightACValue<PowerBacklight::getCurrentBrightness(backlightDevice)) {
            qDebug() << "brightness is higher than ac value, ignore";
            return;
        }
        /*if (hasBacklight) {
            Common::adjustBacklight(backlightDevice, backlightACValue);
        } else {*/
            man->setDisplayBacklight(backlightDevice, backlightACValue);
        //}
    }
}

// load default settings
void SysTray::loadSettings()
{
    qDebug() << "(re)load settings...";

    // set default settings
    if (PowerSettings::isValid(CONF_SUSPEND_BATTERY_TIMEOUT)) {
        autoSuspendBattery = PowerSettings::getValue(CONF_SUSPEND_BATTERY_TIMEOUT).toInt();
    }
    if (PowerSettings::isValid(CONF_SUSPEND_AC_TIMEOUT)) {
        autoSuspendAC = PowerSettings::getValue(CONF_SUSPEND_AC_TIMEOUT).toInt();
    }
    if (PowerSettings::isValid(CONF_SUSPEND_BATTERY_ACTION)) {
        autoSuspendBatteryAction = PowerSettings::getValue(CONF_SUSPEND_BATTERY_ACTION).toInt();
    }
    if (PowerSettings::isValid(CONF_SUSPEND_AC_ACTION)) {
        autoSuspendACAction = PowerSettings::getValue(CONF_SUSPEND_AC_ACTION).toInt();
    }
    if (PowerSettings::isValid(CONF_CRITICAL_BATTERY_TIMEOUT)) {
        critBatteryValue = PowerSettings::getValue(CONF_CRITICAL_BATTERY_TIMEOUT).toInt();
    }
    if (PowerSettings::isValid(CONF_LID_BATTERY_ACTION)) {
        lidActionBattery = PowerSettings::getValue(CONF_LID_BATTERY_ACTION).toInt();
    }
    if (PowerSettings::isValid(CONF_LID_AC_ACTION)) {
        lidActionAC = PowerSettings::getValue(CONF_LID_AC_ACTION).toInt();
    }
    if (PowerSettings::isValid(CONF_CRITICAL_BATTERY_ACTION)) {
        criticalAction = PowerSettings::getValue(CONF_CRITICAL_BATTERY_ACTION).toInt();
    }
    if (PowerSettings::isValid(CONF_FREEDESKTOP_SS)) {
        desktopSS = PowerSettings::getValue(CONF_FREEDESKTOP_SS).toBool();
    }
    if (PowerSettings::isValid(CONF_FREEDESKTOP_PM)) {
        desktopPM = PowerSettings::getValue(CONF_FREEDESKTOP_PM).toBool();
    }
    if (PowerSettings::isValid(CONF_TRAY_NOTIFY)) {
        showNotifications = PowerSettings::getValue(CONF_TRAY_NOTIFY).toBool();
    }
    if (PowerSettings::isValid(CONF_TRAY_SHOW)) {
        showTray = PowerSettings::getValue(CONF_TRAY_SHOW).toBool();
    }
    if (PowerSettings::isValid(CONF_LID_DISABLE_IF_EXTERNAL)) {
        disableLidOnExternalMonitors = PowerSettings::getValue(CONF_LID_DISABLE_IF_EXTERNAL).toBool();
    }
    if (PowerSettings::isValid(CONF_LID_XRANDR)) {
        lidXrandr = PowerSettings::getValue(CONF_LID_XRANDR).toBool();
    }
    if (PowerSettings::isValid(CONF_BACKLIGHT_AC_ENABLE)) {
        backlightOnAC = PowerSettings::getValue(CONF_BACKLIGHT_AC_ENABLE).toBool();
    }
    if (PowerSettings::isValid(CONF_BACKLIGHT_AC)) {
        backlightACValue = PowerSettings::getValue(CONF_BACKLIGHT_AC).toInt();
    }
    if (PowerSettings::isValid(CONF_BACKLIGHT_BATTERY_ENABLE)) {
        backlightOnBattery = PowerSettings::getValue(CONF_BACKLIGHT_BATTERY_ENABLE).toBool();
    }
    if (PowerSettings::isValid(CONF_BACKLIGHT_BATTERY)) {
        backlightBatteryValue = PowerSettings::getValue(CONF_BACKLIGHT_BATTERY).toInt();
    }
    if (PowerSettings::isValid(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER)) {
        backlightBatteryDisableIfLower =  PowerSettings::getValue(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER)
                                                                    .toBool();
    }
    if (PowerSettings::isValid(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER)) {
        backlightACDisableIfHigher = PowerSettings::getValue(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER)
                                                               .toBool();
    }
    if (PowerSettings::isValid(CONF_WARN_ON_LOW_BATTERY)) {
        warnOnLowBattery = PowerSettings::getValue(CONF_WARN_ON_LOW_BATTERY).toBool();
    }
    if (PowerSettings::isValid(CONF_WARN_ON_VERYLOW_BATTERY)) {
        warnOnVeryLowBattery = PowerSettings::getValue(CONF_WARN_ON_VERYLOW_BATTERY).toBool();
    }
    if (PowerSettings::isValid(CONF_NOTIFY_ON_BATTERY)) {
        notifyOnBattery = PowerSettings::getValue(CONF_NOTIFY_ON_BATTERY).toBool();
    }
    if (PowerSettings::isValid(CONF_NOTIFY_ON_AC)) {
        notifyOnAC = PowerSettings::getValue(CONF_NOTIFY_ON_AC).toBool();
    }
    if (PowerSettings::isValid(CONF_SUSPEND_LOCK_SCREEN)) {
        man->setLockScreenOnSuspend(PowerSettings::getValue(CONF_SUSPEND_LOCK_SCREEN).toBool());
    }
    if (PowerSettings::isValid(CONF_RESUME_LOCK_SCREEN)) {
        man->setLockScreenOnResume(PowerSettings::getValue(CONF_RESUME_LOCK_SCREEN).toBool());
    }
    if (PowerSettings::isValid(CONF_SUSPEND_WAKEUP_HIBERNATE_BATTERY)) {
        man->setSuspendWakeAlarmOnBattery(PowerSettings::getValue(CONF_SUSPEND_WAKEUP_HIBERNATE_BATTERY).toInt());
    }
    if (PowerSettings::isValid(CONF_SUSPEND_WAKEUP_HIBERNATE_AC)) {
        man->setSuspendWakeAlarmOnAC(PowerSettings::getValue(CONF_SUSPEND_WAKEUP_HIBERNATE_AC).toInt());
    }

    if (PowerSettings::isValid(CONF_KERNEL_BYPASS)) {
        ignoreKernelResume = PowerSettings::getValue(CONF_KERNEL_BYPASS).toBool();
    } else {
        ignoreKernelResume = false;
    }

    // verify
    /*if (!Common::kernelCanResume(ignoreKernelResume)) {
        qDebug() << "hibernate is not activated in kernel (add resume=...)";
        disableHibernate();
    }*/
    if (!man->CanHibernate()) {
        qDebug() << "hibernate is not supported";
        disableHibernate();
    }
    if (!man->CanSuspend()) {
        qDebug() << "suspend not supported";
        disableSuspend();
    }

    // backlight
    backlightDevice = PowerBacklight::getDevice();
    hasBacklight = PowerBacklight::canAdjustBrightness(backlightDevice);
    if (PowerSettings::isValid(CONF_BACKLIGHT_MOUSE_WHEEL)) {
        backlightMouseWheel = PowerSettings::getValue(CONF_BACKLIGHT_MOUSE_WHEEL).toBool();
    }
}

// register session services
void SysTray::registerService()
{
    if (hasService) { return; }
    if (!QDBusConnection::sessionBus().isConnected()) {
        qWarning("Cannot connect to D-Bus.");
        return;
    }
    if (desktopPM) {
        if (!QDBusConnection::sessionBus().registerService(PM_SERVICE)) {
            qWarning() << QDBusConnection::sessionBus().lastError().message();
            return;
        }
        if (!QDBusConnection::sessionBus().registerObject(PM_PATH,
                                                              pm,
                                                              QDBusConnection::ExportAllSlots)) {
            qWarning() << QDBusConnection::sessionBus().lastError().message();
            return;
        }
        if (!QDBusConnection::sessionBus().registerObject(PM_FULL_PATH,
                                                              pm,
                                                              QDBusConnection::ExportAllSlots)) {
            qWarning() << QDBusConnection::sessionBus().lastError().message();
            return;
        }
        qDebug() << "Enabled org.freedesktop.PowerManagement";
    }
    if (desktopSS) {
        if (!QDBusConnection::sessionBus().registerService(SS_SERVICE)) {
            qWarning() << QDBusConnection::sessionBus().lastError().message();
            return;
        }
        if (!QDBusConnection::sessionBus().registerObject(SS_PATH,
                                                          ss,
                                                          QDBusConnection::ExportAllSlots)) {
            qWarning() << QDBusConnection::sessionBus().lastError().message();
            return;
        }
        if (!QDBusConnection::sessionBus().registerObject(SS_FULL_PATH,
                                                          ss,
                                                          QDBusConnection::ExportAllSlots)) {
            qWarning() << QDBusConnection::sessionBus().lastError().message();
            return;
        }
        qDebug() << "Enabled org.freedesktop.ScreenSaver";
    }
    if (!QDBusConnection::sessionBus().registerService(POWERKIT_SERVICE)) {
        qWarning() << QDBusConnection::sessionBus().lastError().message();
        return;
    }
    if (!QDBusConnection::sessionBus().registerObject(POWERKIT_PATH, man,
                                                      QDBusConnection::ExportAllContents)) {
        qWarning() << QDBusConnection::sessionBus().lastError().message();
        return;
    }
    if (!QDBusConnection::sessionBus().registerObject(POWERKIT_FULL_PATH, man,
                                                      QDBusConnection::ExportAllContents)) {
        qWarning() << QDBusConnection::sessionBus().lastError().message();
        return;
    }
    qDebug() << "Enabled org.freedesktop.PowerKit";
    hasService = true;
}

// dbus session inhibit status handler
void SysTray::handleHasInhibitChanged(bool has_inhibit)
{
    if (has_inhibit) { resetTimer(); }
}

void SysTray::handleLow(double left)
{
    if (!warnOnLowBattery) { return; }
    double batteryLow = (double)(lowBatteryValue+critBatteryValue);
    if (left<=batteryLow && man->OnBattery()) {
        if (!wasLowBattery) {
            showMessage(QString("%1 (%2%)").arg(tr("Low Battery!")).arg(left),
                        tr("The battery is low,"
                           " please consider connecting"
                           " your computer to a power supply."),
                        true);
            wasLowBattery = true;
        }
    }
}

void SysTray::handleVeryLow(double left)
{
    if (!warnOnVeryLowBattery) { return; }
    double batteryVeryLow = (double)(critBatteryValue+1);
    if (left<=batteryVeryLow && man->OnBattery()) {
        if (!wasVeryLowBattery) {
            showMessage(QString("%1 (%2%)").arg(tr("Very Low Battery!")).arg(left),
                        tr("The battery is almost empty,"
                           " please connect"
                           " your computer to a power supply now."),
                        true);
            wasVeryLowBattery = true;
        }
    }
}

// handle critical battery
void SysTray::handleCritical(double left)
{
    if (left<=0 ||
        left>(double)critBatteryValue ||
        !man->OnBattery()) { return; }
    qDebug() << "critical battery!" << criticalAction << left;
    switch(criticalAction) {
    case criticalHibernate:
        man->Hibernate();
        break;
    case criticalShutdown:
        man->PowerOff();
        break;
    default: ;
    }
}

// draw battery tray icon
void SysTray::drawBattery(double left)
{
    if (!showTray &&
        tray->isVisible()) {
        tray->hide();
        return;
    }
    if (tray->isSystemTrayAvailable() &&
        !tray->isVisible() &&
        showTray) { tray->show(); }

    QIcon icon = QIcon::fromTheme(DEFAULT_AC_ICON);
    if (left <= 0 || !man->HasBattery()) {
        tray->setIcon(icon);
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
        if (left >= 100 && !man->OnBattery()) {
            icon = QIcon::fromTheme(DEFAULT_AC_ICON);
        }
    }
    tray->setIcon(icon);
}

// timeout, check if idle
// timeouts and xss must be >= user value and service has to be empty before suspend
void SysTray::timeout()
{
    if (!showTray &&
        tray->isVisible()) { tray->hide(); }
    if (tray->isSystemTrayAvailable() &&
        !tray->isVisible() &&
        showTray) { tray->show(); }

    int uIdle = xIdle();

    qDebug() << "timeout?" << timeouts << "idle?" << uIdle << "inhibit?" << pm->HasInhibit() << pmInhibitors << ssInhibitors;

    int autoSuspend = 0;
    int autoSuspendAction = suspendNone;
    if (man->OnBattery()) {
        autoSuspend = autoSuspendBattery;
        autoSuspendAction = autoSuspendBatteryAction;
    }
    else {
        autoSuspend = autoSuspendAC;
        autoSuspendAction = autoSuspendACAction;
    }

    bool doSuspend = false;
    if (autoSuspend>0 &&
        timeouts>=autoSuspend &&
        uIdle>=autoSuspend &&
        !pm->HasInhibit()) { doSuspend = true; }
    if (!doSuspend) { timeouts++; }
    else {
        timeouts = 0;
        qDebug() << "auto suspend activated" << autoSuspendAction;
        switch (autoSuspendAction) {
        case suspendSleep:
            man->Suspend();
            break;
        case suspendHibernate:
            man->Hibernate();
            break;
        case suspendShutdown:
            man->PowerOff();
            break;
        case suspendHybrid:
            man->HybridSleep();
            break;
        default: break;
        }
    }
}

// get user idle time
int SysTray::xIdle()
{
    long idle = 0;
    Display *display = XOpenDisplay(0);
    if (display != 0) {
        XScreenSaverInfo *info = XScreenSaverAllocInfo();
        XScreenSaverQueryInfo(display, DefaultRootWindow(display), info);
        if (info) {
            idle = info->idle;
            XFree(info);
        }
    }
    XCloseDisplay(display);
    int minutes = (idle-(1000*60))/(1000*60);
    return minutes;
}

// reset the idle timer
void SysTray::resetTimer()
{
    timeouts = 0;
}

// set "internal" monitor
void SysTray::setInternalMonitor()
{
    internalMonitor = Screens::internal();
    qDebug() << "internal monitor set to" << internalMonitor;
}

// is "internal" monitor connected?
bool SysTray::internalMonitorIsConnected()
{
    QMapIterator<QString, bool> i(Screens::outputs());
    while (i.hasNext()) {
        i.next();
        if (i.key() == internalMonitor) {
            qDebug() << "internal monitor connected?" << i.key() << i.value();
            return i.value();
        }
    }
    return false;
}

// is "external" monitor(s) connected?
bool SysTray::externalMonitorIsConnected()
{
    QMapIterator<QString, bool> i(Screens::outputs());
    while (i.hasNext()) {
        i.next();
        if (i.key()!=internalMonitor &&
            !i.key().startsWith(VIRTUAL_MONITOR)) {
            qDebug() << "external monitor connected?" << i.key() << i.value();
            if (i.value()) { return true; }
        }
    }
    return false;
}

// handle new inhibits
void SysTray::handleNewInhibitScreenSaver(const QString &application,
                                          const QString &reason,
                                          quint32 cookie)
{
    qDebug() << "new screensaver inhibit" << application << reason << cookie;
    Q_UNUSED(reason)
    ssInhibitors[cookie] = application;
    checkDevices();
}

void SysTray::handleNewInhibitPowerManagement(const QString &application,
                                              const QString &reason,
                                              quint32 cookie)
{
    qDebug() << "new powermanagement inhibit" << application << reason << cookie;
    Q_UNUSED(reason)
    pmInhibitors[cookie] = application;
    checkDevices();
}

void SysTray::handleDelInhibitScreenSaver(quint32 cookie)
{
    if (ssInhibitors.contains(cookie)) {
        qDebug() << "removed screensaver inhibitor" << ssInhibitors[cookie];
        ssInhibitors.remove(cookie);
        checkDevices();
    }
}

void SysTray::handleDelInhibitPowerManagement(quint32 cookie)
{
    if (pmInhibitors.contains(cookie)) {
        qDebug() << "removed powermanagement inhibitor" << pmInhibitors[cookie];
        pmInhibitors.remove(cookie);
        checkDevices();
    }
}

// what to do when xscreensaver ends
void SysTray::handleScreensaverFinished(int exitcode)
{
    Q_UNUSED(exitcode)
}

// show notifications
void SysTray::showMessage(const QString &title,
                          const QString &msg,
                          bool critical)
{
    if (tray->isVisible() && showNotifications) {
        if (critical) {
            tray->showMessage(title, msg,
                              QSystemTrayIcon::Critical,
                              900000);
        } else {
            tray->showMessage(title, msg);
        }
    }
}

// reload settings if conf changed
void SysTray::handleConfChanged(const QString &file)
{
    Q_UNUSED(file)
    loadSettings();
}

// disable hibernate if enabled
void SysTray::disableHibernate()
{
    if (criticalAction == criticalHibernate) {
        qWarning() << "reset critical action to shutdown";
        criticalAction = criticalShutdown;
        PowerSettings::setValue(CONF_CRITICAL_BATTERY_ACTION,
                                  criticalAction);
    }
    if (lidActionBattery == lidHibernate) {
        qWarning() << "reset lid battery action to lock";
        lidActionBattery = lidLock;
        PowerSettings::setValue(CONF_LID_BATTERY_ACTION,
                                  lidActionBattery);
    }
    if (lidActionAC == lidHibernate) {
        qWarning() << "reset lid ac action to lock";
        lidActionAC = lidLock;
        PowerSettings::setValue(CONF_LID_AC_ACTION,
                                  lidActionAC);
    }
    if (autoSuspendBatteryAction == suspendHibernate) {
        qWarning() << "reset auto suspend battery action to none";
        autoSuspendBatteryAction = suspendNone;
        PowerSettings::setValue(CONF_SUSPEND_BATTERY_ACTION,
                                  autoSuspendBatteryAction);
    }
    if (autoSuspendACAction == suspendHibernate) {
        qWarning() << "reset auto suspend ac action to none";
        autoSuspendACAction = suspendNone;
        PowerSettings::setValue(CONF_SUSPEND_AC_ACTION,
                                  autoSuspendACAction);
    }
}

// disable suspend if enabled
void SysTray::disableSuspend()
{
    if (lidActionBattery == lidSleep) {
        qWarning() << "reset lid battery action to lock";
        lidActionBattery = lidLock;
        PowerSettings::setValue(CONF_LID_BATTERY_ACTION,
                                  lidActionBattery);
    }
    if (lidActionAC == lidSleep) {
        qWarning() << "reset lid ac action to lock";
        lidActionAC = lidLock;
        PowerSettings::setValue(CONF_LID_AC_ACTION,
                                  lidActionAC);
    }
    if (autoSuspendBatteryAction == suspendSleep) {
        qWarning() << "reset auto suspend battery action to none";
        autoSuspendBatteryAction = suspendNone;
        PowerSettings::setValue(CONF_SUSPEND_BATTERY_ACTION,
                                  autoSuspendBatteryAction);
    }
    if (autoSuspendACAction == suspendSleep) {
        qWarning() << "reset auto suspend ac action to none";
        autoSuspendACAction = suspendNone;
        PowerSettings::setValue(CONF_SUSPEND_AC_ACTION,
                                  autoSuspendACAction);
    }
}

// prepare for suspend
void SysTray::handlePrepareForSuspend()
{
    /*qDebug() << "prepare for suspend";
    resetTimer();
    man->releaseSuspendLock();*/
    qDebug() << "do nothing";
}

// prepare for resume
void SysTray::handlePrepareForResume()
{
    qDebug() << "prepare for resume ...";
    resetTimer();
    tray->showMessage(QString(), QString());
    ss->SimulateUserActivity();
}

// turn off/on monitor using xrandr
// optional "hidden" feature (should be handled by a display manager)
void SysTray::switchInternalMonitor(bool toggle)
{
    if (!lidXrandr) { return; }
    qDebug() << "using xrandr to turn on/off internal monitor" << toggle;
    QProcess xrandr;
    xrandr.start(QString(toggle?TURN_ON_MONITOR:TURN_OFF_MONITOR).arg(internalMonitor));
    xrandr.waitForFinished();
    xrandr.close();
}

// adjust backlight on wheel event (on systray)
void SysTray::handleTrayWheel(TrayIcon::WheelAction action)
{
    if (/*!hasBacklight ||*/ !backlightMouseWheel) { return; }
    switch (action) {
    case TrayIcon::WheelUp:
        /*if (hasBacklight) {
            Common::adjustBacklight(backlightDevice,
                                    Common::backlightValue(backlightDevice)+BACKLIGHT_MOVE_VALUE);
        } else {*/
            man->setDisplayBacklight(backlightDevice,
                                     PowerBacklight::getCurrentBrightness(backlightDevice)+BACKLIGHT_MOVE_VALUE);
        //}
        break;
    case TrayIcon::WheelDown:
        /*if (hasBacklight) {
            Common::adjustBacklight(backlightDevice,
                                    Common::backlightValue(backlightDevice)-BACKLIGHT_MOVE_VALUE);
        } else {*/
            man->setDisplayBacklight(backlightDevice,
                                     PowerBacklight::getCurrentBrightness(backlightDevice)-BACKLIGHT_MOVE_VALUE);
        //}
        break;
    default:;
    }
}

// check devices if changed
void SysTray::handleDeviceChanged(const QString &path)
{
    Q_UNUSED(path)
    checkDevices();
}

// close dialog proc if open
void SysTray::handleConfigDialogFinished(int result)
{
    Q_UNUSED(result)
    if (configDialog->isOpen()) { configDialog->close(); }
}

// start dialog proc
void SysTray::showConfigDialog()
{
    if (configDialog->isOpen()) { return; }
    configDialog->start(QString("%1 --config")
                        .arg(qApp->applicationFilePath()));
}

void SysTray::populateMenu()
{
    qDebug() << "populate menu";

    menuFrame = new QFrame(NULL);

    menuFrame->setMaximumWidth(200);
    //menuFrame->setMaximumHeight(500);

    QWidget *batteryWidget = new QWidget(menuFrame);
    QWidget *backlightWidget = new QWidget(menuFrame);

    QVBoxLayout *menuContainerLayout = new QVBoxLayout(menuFrame);
    QHBoxLayout *batteryContainerLayout = new QHBoxLayout(batteryWidget);
    QHBoxLayout *backlightContainerLayout = new QHBoxLayout(backlightWidget);

    batteryWidget->setContentsMargins(0,0,0,0);
    batteryContainerLayout->setContentsMargins(0,0,0,0);
    batteryContainerLayout->setSpacing(0);

    backlightWidget->setContentsMargins(0,0,0,0);
    backlightContainerLayout->setContentsMargins(0,0,0,0);
    backlightContainerLayout->setSpacing(0);

    labelBatteryIcon = new QLabel(batteryWidget);
    labelBatteryStatus = new QLabel(batteryWidget);

    QLabel *backlightLabel = new QLabel(menuFrame);
    backlightLabel->setPixmap(QIcon::fromTheme(DEFAULT_BACKLIGHT_ICON)
                              .pixmap(24, 24));

    backlightSlider = new QSlider(menuFrame);
    backlightSlider->setMinimumWidth(100);
    backlightSlider->setMinimum(1);
    backlightSlider->setMaximum(PowerBacklight::getMaxBrightness(backlightDevice));
    backlightSlider->setSingleStep(1);
    backlightSlider->setOrientation(Qt::Horizontal);
    backlightSlider->setToolTip(tr("Adjust the display brightness."));
    connect(backlightSlider, SIGNAL(valueChanged(int)),
                this, SLOT(handleBacklightSlider(int)));

    powerTab = new QTabWidget(menuFrame);

    deviceTree = new QTreeWidget(menuFrame);
    deviceTree->setStyleSheet("QTreeWidget, QTreeWidget::item,"
                              "QTreeWidget::item:selected"
                              "{ /*background:transparent;*/ border:0; }");
    deviceTree->setHeaderHidden(true);
    //deviceTree->setHeaderLabels(QStringList() << "1" /*<< "2"*/);
    //deviceTree->setColumnWidth(0, 50);
    //deviceTree->setMaximumHeight(30*3);
    //deviceTree->setMinimumHeight(0);
    deviceTree->setRootIsDecorated(false);
    //deviceTree->setMaximumWidth(100);

    inhibitorTree = new QTreeWidget(menuFrame);
    inhibitorTree->setHeaderHidden(true);
    //inhibitorTree->setMaximumHeight(30*3);
    inhibitorTree->setStyleSheet("QTreeWidget {border:0;}");
    inhibitorTree->setRootIsDecorated(false);
    inhibitorTree->hide();

    powerTab->addTab(deviceTree, tr("Batteries"));
    //powerTab->addTab(inhibitorTree, tr("Inhibitors"));
    powerTab->setMaximumHeight(30*4);
    powerTab->setTabPosition(QTabWidget::South);
    //powerTab->hide();

    batteryContainerLayout->addWidget(labelBatteryIcon);
    batteryContainerLayout->addStretch();
    batteryContainerLayout->addWidget(labelBatteryStatus);
    backlightContainerLayout->addWidget(backlightLabel);
    backlightContainerLayout->addWidget(backlightSlider);
    menuContainerLayout->addWidget(batteryWidget);
    //menuContainerLayout->addWidget(deviceTree);
    //menuContainerLayout->addWidget(inhibitorTree);
    menuContainerLayout->addWidget(powerTab);
    menuContainerLayout->addWidget(backlightWidget);

    menuHeader = new QWidgetAction(NULL);
    menuHeader->setDefaultWidget(menuFrame);

    powerMenu->addAction(menuHeader);

    /*actRestart = new QAction(this);
    actSuspend = new QAction(this);
    actPowerOff = new QAction(this);
    actSettings = new QAction(this);
    actHibernate = new QAction(this);

    actRestart->setText(tr("Restart"));
    actSuspend->setText(tr("Suspend"));
    actPowerOff->setText(tr("Shutdown"));
    actSettings->setText(tr("Settings"));
    actHibernate->setText(tr("Hibernate"));

    actRestart->setIcon(QIcon::fromTheme(DEFAULT_SHUTDOWN_ICON));
    actSuspend->setIcon(QIcon::fromTheme(DEFAULT_SUSPEND_ICON));
    actPowerOff->setIcon(QIcon::fromTheme(DEFAULT_SHUTDOWN_ICON));
    actSettings->setIcon(QIcon::fromTheme(DEFAULT_TRAY_ICON));
    actHibernate->setIcon(QIcon::fromTheme(DEFAULT_HIBERNATE_ICON));

    powerMenu->addSeparator();
    powerMenu->addAction(actSuspend);
    powerMenu->addAction(actHibernate);
    powerMenu->addSeparator();
    powerMenu->addAction(actRestart);
    powerMenu->addAction(actPowerOff);
    powerMenu->addSeparator();
    powerMenu->addAction(actSettings);*/

    updateBacklight(QString());
    updateMenu();
}

void SysTray::updateMenu()
{
    qDebug() << "update menu";
    double left = man->BatteryLeft();
    if (left<0) { left = 0; }
    if (left>100) { left = 100; }

    if (man->HasBattery()) {
        QString leftString = QDateTime::fromTime_t(man->OnBattery()?man->TimeToEmpty():man->TimeToFull())
                                .toUTC().toString("hh:mm");
        labelBatteryStatus->setText(QString("<h2 style=\"font-weight:normal;\">%1% (%2)</h2>").arg(left).arg(leftString));
    } else {
        labelBatteryStatus->setText(QString("<h2 style=\"font-weight:normal;\">%1 (00:00)</h2>").arg(tr("AC")));
    }

    QIcon icon = QIcon::fromTheme(DEFAULT_AC_ICON);
    if (left <1 || !man->HasBattery()) {
        labelBatteryIcon->setPixmap(icon.pixmap(QSize(32, 32)));
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
        if (left > 99 && !man->OnBattery()) {
            icon = QIcon::fromTheme(DEFAULT_AC_ICON);
        }
    }
    labelBatteryIcon->setPixmap(icon.pixmap(QSize(32, 32)));
}

void SysTray::updateBacklight(QString file)
{
    qDebug() << "BACKLIGHT SLIDER UPDATE" << file;
    Q_UNUSED(file);
    int value = PowerBacklight::getCurrentBrightness(backlightDevice);
    if (value != backlightSlider->value()) {
        backlightSlider->setValue(value);
    }
}

void SysTray::handleBacklightSlider(int value)
{
    qDebug() << "BACKLIGHT SLIDER CHANGED" << value;
    if (PowerBacklight::getCurrentBrightness(backlightDevice) != value) {
        //if (hasBacklight) { Common::adjustBacklight(backlightDevice, value); }
        /*else {*/ man->setDisplayBacklight(backlightDevice, value); //}
    }
}

void SysTray::updatePowerDevices()
{
    QMapIterator<QString, Device*> i(man->getDevices());
    while (i.hasNext()) {
        i.next();
        //qDebug() << i.value()->name << i.value()->model << i.value()->type  << i.value()->isPresent << i.value()->objectName() << i.value()->percentage;
        QString uid = i.value()->path;
        if (!i.value()->isPresent) {
            if (powerDeviceExists(uid)) { powerDeviceRemove(uid); }
            continue;
        }
        if (!powerDeviceExists(uid)) {
            qDebug() << "NEW POWER DEVICE" << i.value()->name << i.value()->model << i.value()->type;
            QTreeWidgetItem *item = new QTreeWidgetItem(deviceTree);
            item->setToolTip(0, i.value()->model.isEmpty()?i.value()->name:i.value()->model);
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
            devicesProg[uid] = new QProgressBar(menuFrame);
            devicesProg[uid]->setMinimum(0);
            devicesProg[uid]->setMaximum(100);
            devicesProg[uid]->setValue((int)i.value()->percentage);
            deviceTree->setItemWidget(item, 0, devicesProg[uid]);
        } else {
            qDebug() << "UPDATE POWER DEVICE PERCENTAGE" << i.value()->percentage;
            devicesProg[i.value()->path]->setValue((int)i.value()->percentage);
        }
    }
}

bool SysTray::powerDeviceExists(QString uid)
{
    qDebug() << "CONFIG DEVICE EXISTS" << uid;
    for (int i=0;i<deviceTree->topLevelItemCount();++i) {
        QTreeWidgetItem *item = deviceTree->topLevelItem(i);
        if (!item) { continue; }
        if (item->data(0, DEVICE_UUID) == uid) { return true; }
    }
    return false;
}

void SysTray::powerDeviceRemove(QString uid)
{
    qDebug() << "CONFIG DEVICE REMOVE" << uid;
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

void SysTray::getInhibitors()
{
    inhibitorTree->clear();
    QStringList ssList = man->ScreenSaverInhibitors();
    QStringList pmList = man->PowerManagementInhibitors();
    for (int i=0;i<ssList.size();++i) {
        QString inhibitor = ssList.at(i);
        if (inhibitor.isEmpty()) { continue; }
        QTreeWidgetItem *item = new QTreeWidgetItem(inhibitorTree);
        item->setText(0, inhibitor);
        item->setFlags(Qt::ItemIsEnabled);
        item->setIcon(0, QIcon::fromTheme(DEFAULT_TRAY_ICON));
    }
    for (int i=0;i<pmList.size();++i) {
        QString inhibitor = pmList.at(i);
        if (inhibitor.isEmpty()) { continue; }
        QTreeWidgetItem *item = new QTreeWidgetItem(inhibitorTree);
        item->setText(0, inhibitor);
        item->setFlags(Qt::ItemIsEnabled);
        item->setIcon(0, QIcon::fromTheme(DEFAULT_TRAY_ICON));
    }
    if (inhibitorTree->topLevelItemCount()==0) {
        if (!inhibitorTree->isHidden()) {
            inhibitorTree->hide();
        }
        int index = powerTab->indexOf(inhibitorTree);
        if (index>=0) {
            powerTab->removeTab(index);
        }
        if (powerTab->count()==0 && !powerTab->isHidden()) {
            powerTab->hide();
        }
    } else {
        if (inhibitorTree->isHidden()) {
            inhibitorTree->show();
        }
        if (powerTab->count()>0 && powerTab->isHidden()) {
            powerTab->show();
        }
        if (powerTab->indexOf(inhibitorTree)==-1) {
            powerTab->addTab(inhibitorTree, tr("Inhibitors"));
        }
    }
}

// catch wheel events
bool TrayIcon::event(QEvent *e)
{
    if (e->type() == QEvent::Wheel) {
        QWheelEvent *w = (QWheelEvent*)e;
        if (w->orientation() == Qt::Vertical) {
            wheel_delta += w->delta();
            if (abs(wheel_delta)>=120) {
                emit wheel(wheel_delta>0?TrayIcon::WheelUp:TrayIcon::WheelDown);
                wheel_delta = 0;
            }
        }
        return true;
    }
    return QSystemTrayIcon::event(e);
}
