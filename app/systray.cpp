/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "systray.h"
#include "def.h"
#include <QMessageBox>
#include <QApplication>

SysTray::SysTray(QObject *parent)
    : QObject(parent)
    , tray(0)
    , man(0)
    , pm(0)
    , ss(0)
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
    , timer(0)
    , timeouts(0)
    , showNotifications(true)
    , desktopSS(true)
    , desktopPM(true)
    , showTray(true)
    , disableLidOnExternalMonitors(false)
    , autoSuspendBatteryAction(suspendSleep)
    , autoSuspendACAction(suspendNone)
    , xscreensaver(0)
    , startupScreensaver(true)
    , watcher(0)
    , lidXrandr(false)
    , lidWasClosed(false)
    , hasBacklight(false)
    , backlightOnBattery(false)
    , backlightOnAC(false)
    , backlightBatteryValue(0)
    , backlightACValue(0)
    , backlightBatteryDisableIfLower(false)
    , backlightACDisableIfHigher(false)
    , configDialog(0)
    , warnOnLowBattery(true)
    , warnOnVeryLowBattery(true)
    , notifyOnBattery(true)
    , notifyOnAC(true)
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
            SIGNAL(PrepareForSuspend(bool)),
            this,
            SLOT(handlePrepareForSuspend(bool)));
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

    // setup theme
    Common::setIconTheme();
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

    // device check
    QTimer::singleShot(10000,
                       this,
                       SLOT(checkDevices()));
    QTimer::singleShot(1000,
                       this,
                       SLOT(setInternalMonitor()));

    // config dialog
    configDialog = new QProcess(this);
    connect(configDialog,
            SIGNAL(finished(int)),
            this,
            SLOT(handleConfigDialogFinished(int)));

    // setup watcher
    watcher = new QFileSystemWatcher(this);
    watcher->addPath(Common::confDir());
    watcher->addPath(Common::confFile());
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
    if (xscreensaver->isOpen()) { xscreensaver->close(); }
}

// what to do when user clicks systray
void SysTray::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    Q_UNUSED(reason)
    showConfigDialog();
}

void SysTray::checkDevices()
{
    // show/hide tray
    if (tray->isSystemTrayAvailable() &&
        !tray->isVisible() &&
        showTray) { tray->show(); }
    if (!showTray &&
        tray->isVisible()) { tray->hide(); }

    // get battery left and add tooltip
    double batteryLeft = man->BatteryLeft();
    qDebug() << "battery at" << batteryLeft;
    if (batteryLeft > 0 && man->HasBattery()) {
        tray->setToolTip(QString("%1 %2%").arg(tr("Battery at")).arg(batteryLeft));
        if (man->TimeToEmpty()>0 && man->OnBattery()) {
            tray->setToolTip(tray->toolTip()
                             .append(QString(", %1 %2")
                             .arg(QDateTime::fromTime_t(man->TimeToEmpty())
                                                        .toUTC().toString("hh:mm")))
                             .arg(tr("left")));
        }
        if (batteryLeft > 99) { tray->setToolTip(tr("Charged")); }
        if (!man->OnBattery() &&
            man->BatteryLeft() <= 99) {
            if (man->TimeToFull()>0) {
                tray->setToolTip(tray->toolTip()
                                 .append(QString(", %1 %2")
                                 .arg(QDateTime::fromTime_t(man->TimeToFull())
                                                            .toUTC().toString("hh:mm")))
                                 .arg(tr("left")));
            }
            tray->setToolTip(tray->toolTip().append(QString(" (%1)").arg(tr("Charging"))));
        }
    } else { tray->setToolTip(tr("On AC")); }

    // inhibitors tooltip
    if (ssInhibitors.size()>0) {
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
    }

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
    if (hasBacklight &&
        backlightOnBattery &&
        backlightBatteryValue>0) {
        qDebug() << "set brightness on battery";
        if (backlightBatteryDisableIfLower &&
            backlightBatteryValue>Common::backlightValue(backlightDevice)) {
            qDebug() << "brightness is lower than battery value, ignore";
            return;
        }
        Common::adjustBacklight(backlightDevice, backlightBatteryValue);
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
    if (hasBacklight &&
        backlightOnAC &&
        backlightACValue>0) {
        qDebug() << "set brightness on ac";
        if (backlightACDisableIfHigher &&
            backlightACValue<Common::backlightValue(backlightDevice)) {
            qDebug() << "brightness is higher than ac value, ignore";
            return;
        }
        Common::adjustBacklight(backlightDevice, backlightACValue);
    }
}

// load default settings
void SysTray::loadSettings()
{
    qDebug() << "(re)load settings...";

    // set default settings
    if (Common::validPowerSettings(CONF_SUSPEND_BATTERY_TIMEOUT)) {
        autoSuspendBattery = Common::loadPowerSettings(CONF_SUSPEND_BATTERY_TIMEOUT).toInt();
    }
    if (Common::validPowerSettings(CONF_SUSPEND_AC_TIMEOUT)) {
        autoSuspendAC = Common::loadPowerSettings(CONF_SUSPEND_AC_TIMEOUT).toInt();
    }
    if (Common::validPowerSettings(CONF_SUSPEND_BATTERY_ACTION)) {
        autoSuspendBatteryAction = Common::loadPowerSettings(CONF_SUSPEND_BATTERY_ACTION).toInt();
    }
    if (Common::validPowerSettings(CONF_SUSPEND_AC_ACTION)) {
        autoSuspendACAction = Common::loadPowerSettings(CONF_SUSPEND_AC_ACTION).toInt();
    }
    if (Common::validPowerSettings(CONF_CRITICAL_BATTERY_TIMEOUT)) {
        critBatteryValue = Common::loadPowerSettings(CONF_CRITICAL_BATTERY_TIMEOUT).toInt();
    }
    if (Common::validPowerSettings(CONF_LID_BATTERY_ACTION)) {
        lidActionBattery = Common::loadPowerSettings(CONF_LID_BATTERY_ACTION).toInt();
    }
    if (Common::validPowerSettings(CONF_LID_AC_ACTION)) {
        lidActionAC = Common::loadPowerSettings(CONF_LID_AC_ACTION).toInt();
    }
    if (Common::validPowerSettings(CONF_CRITICAL_BATTERY_ACTION)) {
        criticalAction = Common::loadPowerSettings(CONF_CRITICAL_BATTERY_ACTION).toInt();
    }
    if (Common::validPowerSettings(CONF_FREEDESKTOP_SS)) {
        desktopSS = Common::loadPowerSettings(CONF_FREEDESKTOP_SS).toBool();
    }
    if (Common::validPowerSettings(CONF_FREEDESKTOP_PM)) {
        desktopPM = Common::loadPowerSettings(CONF_FREEDESKTOP_PM).toBool();
    }
    if (Common::validPowerSettings(CONF_TRAY_NOTIFY)) {
        showNotifications = Common::loadPowerSettings(CONF_TRAY_NOTIFY).toBool();
    }
    if (Common::validPowerSettings(CONF_TRAY_SHOW)) {
        showTray = Common::loadPowerSettings(CONF_TRAY_SHOW).toBool();
    }
    if (Common::validPowerSettings(CONF_LID_DISABLE_IF_EXTERNAL)) {
        disableLidOnExternalMonitors = Common::loadPowerSettings(CONF_LID_DISABLE_IF_EXTERNAL).toBool();
    }
    if (Common::validPowerSettings(CONF_LID_XRANDR)) {
        lidXrandr = Common::loadPowerSettings(CONF_LID_XRANDR).toBool();
    }
    if (Common::validPowerSettings(CONF_BACKLIGHT_AC_ENABLE)) {
        backlightOnAC = Common::loadPowerSettings(CONF_BACKLIGHT_AC_ENABLE).toBool();
    }
    if (Common::validPowerSettings(CONF_BACKLIGHT_AC)) {
        backlightACValue = Common::loadPowerSettings(CONF_BACKLIGHT_AC).toInt();
    }
    if (Common::validPowerSettings(CONF_BACKLIGHT_BATTERY_ENABLE)) {
        backlightOnBattery = Common::loadPowerSettings(CONF_BACKLIGHT_BATTERY_ENABLE).toBool();
    }
    if (Common::validPowerSettings(CONF_BACKLIGHT_BATTERY)) {
        backlightBatteryValue = Common::loadPowerSettings(CONF_BACKLIGHT_BATTERY).toInt();
    }
    if (Common::validPowerSettings(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER)) {
        backlightBatteryDisableIfLower =  Common::loadPowerSettings(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER)
                                                                    .toBool();
    }
    if (Common::validPowerSettings(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER)) {
        backlightACDisableIfHigher = Common::loadPowerSettings(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER)
                                                               .toBool();
    }
    if (Common::validPowerSettings(CONF_WARN_ON_LOW_BATTERY)) {
        warnOnLowBattery = Common::loadPowerSettings(CONF_WARN_ON_LOW_BATTERY).toBool();
    }
    if (Common::validPowerSettings(CONF_WARN_ON_VERYLOW_BATTERY)) {
        warnOnVeryLowBattery = Common::loadPowerSettings(CONF_WARN_ON_VERYLOW_BATTERY).toBool();
    }
    if (Common::validPowerSettings(CONF_NOTIFY_ON_BATTERY)) {
        notifyOnBattery = Common::loadPowerSettings(CONF_NOTIFY_ON_BATTERY).toBool();
    }
    if (Common::validPowerSettings(CONF_NOTIFY_ON_AC)) {
        notifyOnAC = Common::loadPowerSettings(CONF_NOTIFY_ON_AC).toBool();
    }

    // verify
    if (!Common::kernelCanResume()) {
        qDebug() << "hibernate is not activated in kernel (add resume=...)";
        disableHibernate();
    }
    if (!man->CanHibernate()) {
        qDebug() << "hibernate is not supported";
        disableHibernate();
    }
    if (!man->CanSuspend()) {
        qDebug() << "suspend not supported";
        disableSuspend();
    }

    // backlight
    backlightDevice = Common::backlightDevice();
    hasBacklight = Common::canAdjustBacklight(backlightDevice);
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
                              QSystemTrayIcon::Critical, 900000);
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
    }
    if (lidActionBattery == lidHibernate) {
        qWarning() << "reset lid battery action to lock";
        lidActionBattery = lidLock;
    }
    if (lidActionAC == lidHibernate) {
        qWarning() << "reset lid ac action to lock";
        lidActionAC = lidLock;
    }
    if (autoSuspendBatteryAction == suspendHibernate) {
        qWarning() << "reset auto suspend battery action to none";
        autoSuspendBatteryAction = suspendNone;
    }
    if (autoSuspendACAction == suspendHibernate) {
        qWarning() << "reset auto suspend ac action to none";
        autoSuspendACAction = suspendNone;
    }
}

// disable suspend if enabled
void SysTray::disableSuspend()
{
    if (lidActionBattery == lidSleep) {
        qWarning() << "reset lid battery action to lock";
        lidActionBattery = lidLock;
    }
    if (lidActionAC == lidSleep) {
        qWarning() << "reset lid ac action to lock";
        lidActionAC = lidLock;
    }
    if (autoSuspendBatteryAction == suspendSleep) {
        qWarning() << "reset auto suspend battery action to none";
        autoSuspendBatteryAction = suspendNone;
    }
    if (autoSuspendACAction == suspendSleep) {
        qWarning() << "reset auto suspend ac action to none";
        autoSuspendACAction = suspendNone;
    }
}

// prepare for suspend or resume
void SysTray::handlePrepareForSuspend(bool suspend)
{
    qDebug() << "system prepare for suspend/resume" << suspend;
    man->LockScreen();
    resetTimer();
    if (!suspend) { // resume
        tray->showMessage(QString(), QString());
        man->UpdateDevices();
        ss->SimulateUserActivity();
    }
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
    if (!hasBacklight) { return; }
    switch (action) {
    case TrayIcon::WheelUp:
        Common::adjustBacklight(backlightDevice,
                                Common::backlightValue(backlightDevice)+10);
        break;
    case TrayIcon::WheelDown:
        Common::adjustBacklight(backlightDevice,
                                Common::backlightValue(backlightDevice)-10);
        break;
    default: ;
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
