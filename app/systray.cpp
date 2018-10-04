/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
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
    , pd(0)
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
    , disableLidOnExternalMonitors(true)
    , autoSuspendBatteryAction(suspendSleep)
    , autoSuspendACAction(suspendNone)
    , xscreensaver(0)
    , startupScreensaver(true)
    , watcher(0)
    , lidXrandr(false)
    , lidWasClosed(false)
    , hasBacklight(false)
{
    // setup watcher
    watcher = new QFileSystemWatcher(this);
    watcher->addPath(Common::confFile());
    watcher->addPath(Common::confDir());
    connect(watcher, SIGNAL(fileChanged(QString)),
            this, SLOT(handleConfChanged(QString)));
    connect(watcher, SIGNAL(directoryChanged(QString)),
            this, SLOT(handleConfChanged(QString)));

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
    man = new Power(this);
    connect(man,
            SIGNAL(updatedDevices()),
            this,
            SLOT(checkDevices()));
    connect(man,
            SIGNAL(closedLid()),
            this,
            SLOT(handleClosedLid()));
    connect(man,
            SIGNAL(openedLid()),
            this,
            SLOT(handleOpenedLid()));
    connect(man,
            SIGNAL(switchedToBattery()),
            this,
            SLOT(handleOnBattery()));
    connect(man,
            SIGNAL(switchedToAC()),
            this,
            SLOT(handleOnAC()));
    /*connect(man,
            SIGNAL(notifyStatus(QString,QString,bool)),
            this,
            SLOT(showMessage(QString,QString,bool)));*/
    connect(man,
            SIGNAL(aboutToResume()),
            this,
            SLOT(handleResume()));
    connect(man,
            SIGNAL(aboutToSuspend()),
            this,
            SLOT(handleSuspend()));

    // setup org.freedesktop.PowerManagement
    pm = new PowerManagement();
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

    // setup org.freedesktop.ScreenSaver
    ss = new ScreenSaver();
    connect(ss,
            SIGNAL(newInhibit(QString,QString,quint32)),
            this,
            SLOT(handleNewInhibitScreenSaver(QString,QString,quint32)));
    connect(ss,
            SIGNAL(removedInhibit(quint32)),
            this,
            SLOT(handleDelInhibitScreenSaver(quint32)));

    // setup org.freedesktop.PowerDwarf
    pd = new PowerDwarf();
    connect(pd,
            SIGNAL(update()),
            this,
            SLOT(loadSettings()));
    connect(ss,
            SIGNAL(newInhibit(QString,QString,quint32)),
            pd,
            SLOT(handleNewInhibitScreenSaver(QString,QString,quint32)));
    connect(ss,
            SIGNAL(removedInhibit(quint32)),
            pd,
            SLOT(handleDelInhibitScreenSaver(quint32)));
    connect(pm,
            SIGNAL(newInhibit(QString,QString,quint32)),
            pd,
            SLOT(handleNewInhibitPowerManagement(QString,QString,quint32)));
    connect(pm,
            SIGNAL(removedInhibit(quint32)),
            pd,
            SLOT(handleDelInhibitPowerManagement(quint32)));

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
        qDebug() << "run xscreensaver";
        xscreensaver->start(XSCREENSAVER_RUN);
    }

    // device check
    QTimer::singleShot(10000,
                       this,
                       SLOT(checkDevices()));
    QTimer::singleShot(1000,
                       this,
                       SLOT(setInternalMonitor()));
}

SysTray::~SysTray()
{
    if (xscreensaver->isOpen()) { xscreensaver->close(); }
    pm->deleteLater();
    ss->deleteLater();
    pd->deleteLater();
}

// what to do when user clicks systray
void SysTray::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    Q_UNUSED(reason)
    QString config = QString("%1 --config").arg(qApp->applicationFilePath());
    QProcess::startDetached(config);
}

void SysTray::checkDevices()
{
    // show/hide tray
    if (tray->isSystemTrayAvailable() &&
        !tray->isVisible() &&
        showTray) { tray->show(); }
    if (!showTray &&
        tray->isVisible()) { tray->hide(); }

    // warn if systray has no icons
    if (tray->isVisible() && (QIcon::themeName().isEmpty() ||
        QIcon::themeName() == "hicolor")) {
        showMessage(tr("No icon theme found!"),
                    tr("Unable to find any icon theme,"
                       " please install a theme and restart powerdwarf."),
                    true);
    }

    // get battery left and add tooltip
    double batteryLeft = man->batteryLeft();
    if (batteryLeft > 0) {
        tray->setToolTip(tr("Battery at %1%").arg(batteryLeft));
        if (batteryLeft > 99) { tray->setToolTip(tr("Charged")); }
        if (!man->onBattery() &&
            man->batteryLeft() <= 99)
        { tray->setToolTip(tray->toolTip().append(tr(" (Charging)"))); }
    } else { tray->setToolTip(tr("On AC")); }

    // draw battery systray
    drawBattery(batteryLeft);

    // low battery?
    handleLow(batteryLeft);

    // very low battery?
    handleVeryLow(batteryLeft);

    // critical battery?
    if (batteryLeft > 0 &&
        batteryLeft<=(double)critBatteryValue &&
        man->onBattery()) { handleCritical(); }

    // Register service if not already registered
    if (!hasService) { registerService(); }
}

// what to do when user close lid
void SysTray::handleClosedLid()
{
    qDebug() << "lid closed";
    lidWasClosed = true;

    int type = lidNone;
    if (man->onBattery()) {  // on battery
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
        man->lockScreen();
        break;
    case lidSleep:
        man->sleep();
        break;
    case lidHibernate:
        man->hibernate();
        break;
    case lidShutdown:
        man->shutdown();
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
    showMessage(tr("On Battery"),
                tr("Switched to battery power."));
    // TODO: add brightness
}

// do something when switched to ac power
void SysTray::handleOnAC()
{
    showMessage(tr("On AC"),
                tr("Switched to AC power."));

    wasLowBattery = false;
    wasVeryLowBattery = false;
    // TODO: add brightness
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

    // verify
    if (!Common::kernelCanResume()) {
        qDebug() << "hibernate is not activated in kernel (add resume=...)";
        disableHibernate();
    }
    if (!man->canHibernate()) {
        qDebug() << "hibernate is not supported";
        disableHibernate();
    }
    if (!man->canSuspend()) {
        qDebug() << "suspend not supported";
        disableSuspend();
    }

    // backlight
    backlightDevice = Common::backlightDevice();
    hasBacklight = Common::canAdjustBacklight(backlightDevice);

    qDebug() << CONF_LID_XRANDR << lidXrandr;
    qDebug() << CONF_LID_DISABLE_IF_EXTERNAL << disableLidOnExternalMonitors;
    qDebug() << CONF_TRAY_SHOW << showTray;
    qDebug() << CONF_TRAY_NOTIFY << showNotifications;
    qDebug() << CONF_FREEDESKTOP_SS << desktopSS;
    qDebug() << CONF_FREEDESKTOP_PM << desktopPM;
    qDebug() << CONF_SUSPEND_BATTERY_TIMEOUT << autoSuspendBattery;
    qDebug() << CONF_SUSPEND_AC_TIMEOUT << autoSuspendAC;
    qDebug() << CONF_SUSPEND_BATTERY_ACTION << autoSuspendBatteryAction;
    qDebug() << CONF_SUSPEND_AC_ACTION << autoSuspendACAction;
    qDebug() << CONF_CRITICAL_BATTERY_TIMEOUT << critBatteryValue;
    qDebug() << CONF_LID_BATTERY_ACTION << lidActionBattery;
    qDebug() << CONF_LID_AC_ACTION << lidActionAC;
    qDebug() << CONF_CRITICAL_BATTERY_ACTION << criticalAction;

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
        qDebug() << "Enabled org.freedesktop.ScreenSaver";
    }
    if (!QDBusConnection::sessionBus().registerService(PD_SERVICE)) {
        qWarning() << QDBusConnection::sessionBus().lastError().message();
        return;
    }
    if (!QDBusConnection::sessionBus().registerObject(PD_PATH,
                                                      pd,
                                                      QDBusConnection::ExportAllContents)) {
        qWarning() << QDBusConnection::sessionBus().lastError().message();
        return;
    }
    qDebug() << "Enabled org.freedesktop.PowerDwarf";
    hasService = true;
}

// dbus session inhibit status handler
void SysTray::handleHasInhibitChanged(bool has_inhibit)
{
    if (has_inhibit) { resetTimer(); }
}

void SysTray::handleLow(double left)
{
    double batteryLow = (double)(lowBatteryValue+critBatteryValue);
    if (left<=batteryLow && man->onBattery()) {
        if (!wasLowBattery) {
            showMessage(tr("Low Battery! (%1%)").arg(left),
                        tr("You battery is low,"
                           " please consider connecting"
                           " your computer to a power supply."),
                        true);
            wasLowBattery = true;
        }
    }
}

void SysTray::handleVeryLow(double left)
{
    double batteryVeryLow = (double)(critBatteryValue+1);
    if (left<=batteryVeryLow && man->onBattery()) {
        if (!wasVeryLowBattery) {
            showMessage(tr("Very Low Battery! (%1%)").arg(left),
                        tr("You battery is almost empty,"
                           " please connect"
                           " your computer to a power supply."),
                        true);
            wasVeryLowBattery = true;
        }
    }
}

// handle critical battery
void SysTray::handleCritical()
{
    qDebug() << "critical battery" << criticalAction;
    switch(criticalAction) {
    case criticalHibernate:
        man->hibernate();
        break;
    case criticalShutdown:
        man->shutdown();
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

    QIcon icon = QIcon::fromTheme(DEFAULT_BATTERY_ICON);
    if (left == 0.0) {
        icon = QIcon::fromTheme(DEFAULT_AC_ICON);
        tray->setIcon(icon);
        return;
    }

    if (left<= 10) {
        icon = QIcon::fromTheme(man->onBattery()?DEFAULT_BATTERY_ICON_CRIT:DEFAULT_BATTERY_ICON_CRIT_AC);
    } else if (left<=25) {
        icon = QIcon::fromTheme(man->onBattery()?DEFAULT_BATTERY_ICON_LOW:DEFAULT_BATTERY_ICON_LOW_AC);
    } else if (left<=75) {
        icon = QIcon::fromTheme(man->onBattery()?DEFAULT_BATTERY_ICON_GOOD:DEFAULT_BATTERY_ICON_GOOD_AC);
    } else if (left<=90) {
        icon = QIcon::fromTheme(man->onBattery()?DEFAULT_BATTERY_ICON_FULL:DEFAULT_BATTERY_ICON_FULL_AC);
    } else {
        icon = QIcon::fromTheme(man->onBattery()?DEFAULT_BATTERY_ICON_FULL:DEFAULT_BATTERY_ICON_CHARGED);
        if (left>=100 && !man->onBattery()) {
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
    if (man->onBattery()) {
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
            man->sleep();
            break;
        case suspendHibernate:
            man->hibernate();
            break;
        case suspendShutdown:
            man->shutdown();
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
    int hours = idle/(1000*60*60);
    int minutes = (idle-(hours*1000*60*60))/(1000*60);
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
void SysTray::handleNewInhibitScreenSaver(QString application, QString reason, quint32 cookie)
{
    qDebug() << "new screensaver inhibit" << application << reason << cookie;
    Q_UNUSED(reason)
    ssInhibitors[cookie] = application;
}

void SysTray::handleNewInhibitPowerManagement(QString application, QString reason, quint32 cookie)
{
    qDebug() << "new powermanagement inhibit" << application << reason << cookie;
    Q_UNUSED(reason)
    pmInhibitors[cookie] = application;
}

void SysTray::handleDelInhibitScreenSaver(quint32 cookie)
{
    if (ssInhibitors.contains(cookie)) {
        qDebug() << "removed screensaver inhibitor" << ssInhibitors[cookie];
        ssInhibitors.remove(cookie);
    }
}

void SysTray::handleDelInhibitPowerManagement(quint32 cookie)
{
    if (pmInhibitors.contains(cookie)) {
        qDebug() << "removed powermanagement inhibitor" << pmInhibitors[cookie];
        pmInhibitors.remove(cookie);
    }
}

// what to do when xscreensaver ends
void SysTray::handleScreensaverFinished(int exitcode)
{
    Q_UNUSED(exitcode)
}

// show notifications
void SysTray::showMessage(QString title, QString msg, bool critical)
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
void SysTray::handleConfChanged(QString file)
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

void SysTray::handleResume()
{
    //qDebug() << "handle resume";
}

void SysTray::handleSuspend()
{
    qDebug() << "reset timer on suspend";
    resetTimer();
}

void SysTray::switchInternalMonitor(bool toggle)
{
    if (!lidXrandr) { return; }
    qDebug() << "using xrandr to turn on/off internal monitor" << toggle;
    QProcess xrandr;
    xrandr.start(QString(toggle?TURN_ON_MONITOR:TURN_OFF_MONITOR).arg(internalMonitor));
    xrandr.waitForFinished();
    xrandr.close();
}

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
