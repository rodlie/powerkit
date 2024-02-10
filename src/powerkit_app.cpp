/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit_app.h"
#include "powerkit_common.h"
#include "powerkit_theme.h"
#include "powerkit_notify.h"
#include "powerkit_settings.h"
#include "powerkit_backlight.h"

#include "InhibitAdaptor.h"
#include "ScreenSaverAdaptor.h"

#include <QMessageBox>
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QMapIterator>

#define VIRTUAL_MONITOR "VIRTUAL"

#define PK_SCREENSAVER_SERVICE "org.freedesktop.ScreenSaver"
#define PK_SCREENSAVER_PATH_ROOT "/ScreenSaver"
#define PK_SCREENSAVER_PATH_FULL "/org/freedesktop/ScreenSaver"

#define PM_SERVICE_INHIBIT "org.freedesktop.PowerManagement.Inhibit"
#define PM_FULL_PATH_INHIBIT "/org/freedesktop/PowerManagement/Inhibit"

#define PM_SERVICE "org.freedesktop.PowerManagement"
#define PM_FULL_PATH "/org/freedesktop/PowerManagement"
#define POWERKIT_FULL_PATH "/org/freedesktop/PowerKit"

using namespace PowerKit;

App::App(QObject *parent)
    : QObject(parent)
    , tray(nullptr)
    , man(nullptr)
    , pm(nullptr)
    , ss(nullptr)
    , wasLowBattery(false)
    , wasVeryLowBattery(false)
    , lowBatteryValue(POWERKIT_LOW_BATTERY)
    , critBatteryValue(POWERKIT_CRITICAL_BATTERY)
    , hasService(false)
    , lidActionBattery(POWERKIT_LID_BATTERY_ACTION)
    , lidActionAC(POWERKIT_LID_AC_ACTION)
    , criticalAction(POWERKIT_CRITICAL_ACTION)
    , autoSuspendBattery(POWERKIT_AUTO_SLEEP_BATTERY)
    , autoSuspendAC(0)
    , timer(nullptr)
    , timeouts(0)
    , showNotifications(true)
    , showTray(true)
    , disableLidOnExternalMonitors(false)
    , autoSuspendBatteryAction(POWERKIT_SUSPEND_BATTERY_ACTION)
    , autoSuspendACAction(POWERKIT_SUSPEND_AC_ACTION)
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
    , warnOnLowBattery(true)
    , warnOnVeryLowBattery(true)
    , notifyOnBattery(true)
    , notifyOnAC(true)
    , notifyNewInhibitor(true)
    , backlightMouseWheel(true)
    , ignoreKernelResume(false)
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

    // setup org.freedesktop.PowerKit.Manager
    man = new Manager(this);
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
            SIGNAL(Error(QString)),
            this,
            SLOT(handleError(QString)));
    connect(man,
            SIGNAL(Warning(QString)),
            this,
            SLOT(handleWarning(QString)));

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

    // setup timer
    timer = new QTimer(this);
    timer->setInterval(60000);
    connect(timer,
            SIGNAL(timeout()),
            this,
            SLOT(timeout()));
    timer->start();

    // check for config
    Settings::getConf();

    // setup theme
    Theme::setAppTheme();
    Theme::setIconTheme();
    if (tray->icon().isNull()) {
        tray->setIcon(QIcon::fromTheme(POWERKIT_ICON));
    }

    loadSettings();
    registerService();

    // device check
    QTimer::singleShot(10000,
                       this,
                       SLOT(checkDevices()));
    QTimer::singleShot(1000,
                       this,
                       SLOT(setInternalMonitor()));

    // setup watcher
    watcher = new QFileSystemWatcher(this);
    watcher->addPath(Settings::getDir());
    watcher->addPath(Settings::getConf());
    connect(watcher,
            SIGNAL(fileChanged(QString)),
            this,
            SLOT(handleConfChanged(QString)));
    connect(watcher,
            SIGNAL(directoryChanged(QString)),
            this,
            SLOT(handleConfChanged(QString)));
}

App::~App()
{
}

void App::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    Q_UNUSED(reason)

    openSettings();
}

void App::checkDevices()
{
    updateTrayVisibility();

    double batteryLeft = man->BatteryLeft();
    qDebug() << "battery at" << batteryLeft;

    drawBattery(batteryLeft);
    updateToolTip();

    handleLow(batteryLeft);
    handleVeryLow(batteryLeft);
    handleCritical(batteryLeft);
}

void App::handleClosedLid()
{
    qDebug() << "lid closed";
    lidWasClosed = true;

    int type = lidNone;
    if (man->OnBattery()) {
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
    case lidSleepHibernate:
        man->SuspendThenHibernate();
        break;
    default:;
    }
}

void App::handleOpenedLid()
{
    qDebug() << "lid is now open";
    lidWasClosed = false;
    if (disableLidOnExternalMonitors) {
        switchInternalMonitor(true /* turn on screen */);
    }
}

void App::handleOnBattery()
{
    if (notifyOnBattery) {
        showMessage(tr("On Battery"),
                    tr("Switched to battery power."));
    }

    // brightness
    if (backlightOnBattery && backlightBatteryValue > 0) {
        qDebug() << "set brightness on battery";
        if (backlightBatteryDisableIfLower &&
            backlightBatteryValue > Backlight::getCurrentBrightness(backlightDevice)) {
            qDebug() << "brightness is lower than battery value, ignore";
            return;
        }
        Backlight::setBrightness(backlightDevice,
                                 backlightBatteryValue);
    }
}

void App::handleOnAC()
{
    if (notifyOnAC) {
        showMessage(tr("On AC"),
                    tr("Switched to AC power."));
    }

    wasLowBattery = false;
    wasVeryLowBattery = false;

    // brightness
    if (backlightOnAC && backlightACValue > 0) {
        qDebug() << "set brightness on ac";
        if (backlightACDisableIfHigher &&
            backlightACValue < Backlight::getCurrentBrightness(backlightDevice)) {
            qDebug() << "brightness is higher than ac value, ignore";
            return;
        }
        Backlight::setBrightness(backlightDevice,
                                 backlightACValue);
    }
}

void App::loadSettings()
{
    qDebug() << "(re)load settings...";

    // set default settings
    if (Settings::isValid(CONF_SUSPEND_BATTERY_TIMEOUT)) {
        autoSuspendBattery = Settings::getValue(CONF_SUSPEND_BATTERY_TIMEOUT).toInt();
    }
    if (Settings::isValid(CONF_SUSPEND_AC_TIMEOUT)) {
        autoSuspendAC = Settings::getValue(CONF_SUSPEND_AC_TIMEOUT).toInt();
    }
    if (Settings::isValid(CONF_SUSPEND_BATTERY_ACTION)) {
        autoSuspendBatteryAction = Settings::getValue(CONF_SUSPEND_BATTERY_ACTION).toInt();
    }
    if (Settings::isValid(CONF_SUSPEND_AC_ACTION)) {
        autoSuspendACAction = Settings::getValue(CONF_SUSPEND_AC_ACTION).toInt();
    }
    if (Settings::isValid(CONF_CRITICAL_BATTERY_TIMEOUT)) {
        critBatteryValue = Settings::getValue(CONF_CRITICAL_BATTERY_TIMEOUT).toInt();
    }
    if (Settings::isValid(CONF_LID_BATTERY_ACTION)) {
        lidActionBattery = Settings::getValue(CONF_LID_BATTERY_ACTION).toInt();
    }
    if (Settings::isValid(CONF_LID_AC_ACTION)) {
        lidActionAC = Settings::getValue(CONF_LID_AC_ACTION).toInt();
    }
    if (Settings::isValid(CONF_CRITICAL_BATTERY_ACTION)) {
        criticalAction = Settings::getValue(CONF_CRITICAL_BATTERY_ACTION).toInt();
    }
    if (Settings::isValid(CONF_TRAY_NOTIFY)) {
        showNotifications = Settings::getValue(CONF_TRAY_NOTIFY).toBool();
    }
    if (Settings::isValid(CONF_TRAY_SHOW)) {
        showTray = Settings::getValue(CONF_TRAY_SHOW).toBool();
    }
    if (Settings::isValid(CONF_LID_DISABLE_IF_EXTERNAL)) {
        disableLidOnExternalMonitors = Settings::getValue(CONF_LID_DISABLE_IF_EXTERNAL).toBool();
    }
    if (Settings::isValid(CONF_LID_XRANDR)) {
        lidXrandr = Settings::getValue(CONF_LID_XRANDR).toBool();
    }
    if (Settings::isValid(CONF_BACKLIGHT_AC_ENABLE)) {
        backlightOnAC = Settings::getValue(CONF_BACKLIGHT_AC_ENABLE).toBool();
    }
    if (Settings::isValid(CONF_BACKLIGHT_AC)) {
        backlightACValue = Settings::getValue(CONF_BACKLIGHT_AC).toInt();
    }
    if (Settings::isValid(CONF_BACKLIGHT_BATTERY_ENABLE)) {
        backlightOnBattery = Settings::getValue(CONF_BACKLIGHT_BATTERY_ENABLE).toBool();
    }
    if (Settings::isValid(CONF_BACKLIGHT_BATTERY)) {
        backlightBatteryValue = Settings::getValue(CONF_BACKLIGHT_BATTERY).toInt();
    }
    if (Settings::isValid(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER)) {
        backlightBatteryDisableIfLower =  Settings::getValue(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER)
                                                             .toBool();
    }
    if (Settings::isValid(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER)) {
        backlightACDisableIfHigher = Settings::getValue(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER)
                                                        .toBool();
    }
    if (Settings::isValid(CONF_WARN_ON_LOW_BATTERY)) {
        warnOnLowBattery = Settings::getValue(CONF_WARN_ON_LOW_BATTERY).toBool();
    }
    if (Settings::isValid(CONF_WARN_ON_VERYLOW_BATTERY)) {
        warnOnVeryLowBattery = Settings::getValue(CONF_WARN_ON_VERYLOW_BATTERY).toBool();
    }
    if (Settings::isValid(CONF_NOTIFY_ON_BATTERY)) {
        notifyOnBattery = Settings::getValue(CONF_NOTIFY_ON_BATTERY).toBool();
    }
    if (Settings::isValid(CONF_NOTIFY_ON_AC)) {
        notifyOnAC = Settings::getValue(CONF_NOTIFY_ON_AC).toBool();
    }
    if (Settings::isValid(CONF_NOTIFY_NEW_INHIBITOR)) {
        notifyNewInhibitor = Settings::getValue(CONF_NOTIFY_NEW_INHIBITOR).toBool();
    }
    /*if (Settings::isValid(CONF_SUSPEND_WAKEUP_HIBERNATE_BATTERY)) {
        man->SetSuspendWakeAlarmOnBattery(Settings::getValue(CONF_SUSPEND_WAKEUP_HIBERNATE_BATTERY).toInt());
    }
    if (Settings::isValid(CONF_SUSPEND_WAKEUP_HIBERNATE_AC)) {
        man->SetSuspendWakeAlarmOnAC(Settings::getValue(CONF_SUSPEND_WAKEUP_HIBERNATE_AC).toInt());
    }*/

    if (Settings::isValid(CONF_KERNEL_BYPASS)) {
        ignoreKernelResume = Settings::getValue(CONF_KERNEL_BYPASS).toBool();
    } else {
        ignoreKernelResume = false;
    }

    // verify
    /*if (!Common::kernelCanResume(ignoreKernelResume)) {
        qDebug() << "hibernate is not activated in kernel (add resume=...)";
        disableHibernate();
    }*/
    if (!man->CanHibernate()) {
        qWarning() << "hibernate is not supported";
        disableHibernate();
    }
    if (!man->CanSuspend()) {
        qWarning() << "suspend not supported";
        disableSuspend();
    }

    // backlight
    backlightDevice = Backlight::getDevice();
    hasBacklight = Backlight::canAdjustBrightness(backlightDevice);
    if (Settings::isValid(CONF_BACKLIGHT_MOUSE_WHEEL)) {
        backlightMouseWheel = Settings::getValue(CONF_BACKLIGHT_MOUSE_WHEEL).toBool();
    }

    // screensaver
    ss->Update();
}

void App::registerService()
{
    if (hasService) { return; }
    if (!QDBusConnection::sessionBus().isConnected()) {
        qWarning() << tr("Cannot connect to D-Bus.");
        return;
    }
    hasService = true;

    // register org.freedesktop.PowerManagement
    bool hasDesktopPM = true;
    if (!QDBusConnection::sessionBus().registerService(PM_SERVICE)) {
        qWarning() << QDBusConnection::sessionBus().lastError().message();
        hasDesktopPM = false;
    } else {
        if (!QDBusConnection::sessionBus().registerService(PM_SERVICE_INHIBIT)) {
            qWarning() << QDBusConnection::sessionBus().lastError().message();
            hasDesktopPM = false;
        } else {
            new InhibitAdaptor(pm);
            if (!QDBusConnection::sessionBus().registerObject(PM_FULL_PATH_INHIBIT, pm)) {
                qWarning() << QDBusConnection::sessionBus().lastError().message();
                hasDesktopPM = false;
            }
        }
    }
    qWarning() << "Enabled org.freedesktop.PowerManagement" << hasDesktopPM;

    // register org.freedesktop.ScreenSaver
    bool hasScreenSaver = true;
    if (!QDBusConnection::sessionBus().registerService(PK_SCREENSAVER_SERVICE)) {
        qWarning() << "Failed to register screensaver service" << QDBusConnection::sessionBus().lastError().message();
        hasScreenSaver = false;
    } else {
        new ScreenSaverAdaptor(ss);
        if (!QDBusConnection::sessionBus().registerObject(PK_SCREENSAVER_PATH_ROOT, ss)) {
            qWarning() << "Failed to register screensaver object" << QDBusConnection::sessionBus().lastError().message();
            hasScreenSaver = false;
        }
        if (!QDBusConnection::sessionBus().registerObject(PK_SCREENSAVER_PATH_FULL, ss)) {
            qWarning() << "Failed to register screensaver object" << QDBusConnection::sessionBus().lastError().message();
            hasScreenSaver = false;
        }
    }
    qWarning() << "Enabled org.freedesktop.ScreenSaver" << hasScreenSaver;

    bool hasDesktopPK = true;
    if (!QDBusConnection::sessionBus().registerService(POWERKIT_SERVICE)) {
        qWarning() << QDBusConnection::sessionBus().lastError().message();
        hasDesktopPK = false;
    }
    if (!QDBusConnection::sessionBus().registerObject(POWERKIT_PATH,
                                                      man,
                                                      QDBusConnection::ExportAllContents)) {
        qWarning() << QDBusConnection::sessionBus().lastError().message();
        hasDesktopPK = false;
    }
    if (!QDBusConnection::sessionBus().registerObject(POWERKIT_FULL_PATH,
                                                      man,
                                                      QDBusConnection::ExportAllContents)) {
        qWarning() << QDBusConnection::sessionBus().lastError().message();
        hasDesktopPK = false;
    }
    qWarning() << "Enabled org.freedesktop.PowerKit" << hasDesktopPK;

    if (!hasDesktopPK || !hasDesktopPM || !hasScreenSaver) { hasService = false; }

    if (!hasService) { handleError(tr("Failed to setup and/or connect required services!")); }
}

// dbus session inhibit status handler
void App::handleHasInhibitChanged(bool has_inhibit)
{
    if (has_inhibit) { resetTimer(); }
}

void App::handleLow(double left)
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

void App::handleVeryLow(double left)
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

void App::handleCritical(double left)
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
    case criticalSuspend:
        man->Suspend();
        break;
    default: ;
    }
}

void App::drawBattery(double left)
{
    if (!showTray &&
        tray->isVisible()) {
        tray->hide();
        return;
    }
    if (tray->isSystemTrayAvailable() &&
        !tray->isVisible() &&
        showTray) { tray->show(); }

    QColor colorBg = Qt::green;
    QColor colorFg = Qt::white;
    int pixelSize = 22;
    if (man->OnBattery()) {
        if (left >= 26) {
            colorBg = QColor("orange");
        } else {
            colorBg = Qt::red;
        }
    } else {
        if (!man->HasBattery()) { left = 100; }
        else if (left == 100) { colorFg = Qt::green; }
    }

    tray->setIcon(Theme::drawCircleProgress(left,
                                            pixelSize,
                                            4,
                                            4,
                                            false,
                                            QString(),
                                            colorBg,
                                            colorFg));
}

void App::updateToolTip()
{
    if (!tray->isSystemTrayAvailable() || !tray->isVisible()) { return; }
    double batteryLeft = man->BatteryLeft();
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
}

void App::updateTrayVisibility()
{
    if (!showTray &&
        tray->isVisible()) { tray->hide(); }
    if (tray->isSystemTrayAvailable() &&
        !tray->isVisible() &&
        showTray) { tray->show(); }
}

// timeout, check if idle
// timeouts and xss must be >= user value and service has to be empty before suspend
void App::timeout()
{
    updateTrayVisibility();

    int uIdle = ss->GetSessionIdleTime() / 60;

    qDebug() << "timeout?" << timeouts << "idle?" << uIdle << "inhibit?" << pm->HasInhibit() << man->GetInhibitors();

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
        case suspendSleepHibernate:
            man->SuspendThenHibernate();
            break;
        default:;
        }
    }
}

void App::resetTimer()
{
    timeouts = 0;
}

void App::setInternalMonitor()
{
    internalMonitor = ss->GetInternalDisplay();
    qDebug() << "internal monitor set to" << internalMonitor;
    qDebug() << ss->GetDisplays();
}

bool App::internalMonitorIsConnected()
{
    QMapIterator<QString, bool> i(ss->GetDisplays());
    while (i.hasNext()) {
        i.next();
        if (i.key() == internalMonitor) {
            qDebug() << "internal monitor connected?" << i.key() << i.value();
            return i.value();
        }
    }
    return false;
}

bool App::externalMonitorIsConnected()
{
    QMapIterator<QString, bool> i(ss->GetDisplays());
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

void App::handleNewInhibitScreenSaver(const QString &application,
                                      const QString &reason,
                                      quint32 cookie)
{
    Q_UNUSED(cookie)
    if (notifyNewInhibitor) {
        showMessage(tr("New screen inhibitor"),
                    QString("%1: %2").arg(application, reason));
    }

    checkDevices();
}

void App::handleNewInhibitPowerManagement(const QString &application,
                                          const QString &reason,
                                          quint32 cookie)
{
    Q_UNUSED(cookie)
    if (notifyNewInhibitor) {
        showMessage(tr("New power inhibitor"),
                    QString("%1: %2").arg(application, reason));
    }

    checkDevices();
}

void App::handleDelInhibitScreenSaver(quint32 cookie)
{
    Q_UNUSED(cookie)
    checkDevices();
}

void App::handleDelInhibitPowerManagement(quint32 cookie)
{
    Q_UNUSED(cookie)
    checkDevices();
}

void App::showMessage(const QString &title,
                      const QString &msg,
                      bool critical)
{
    if (!showNotifications) { return; }
    SystemNotification notifier;
    if (notifier.valid) {
        notifier.sendMessage(title, msg, critical);
    } else if (tray->isVisible()) {
        if (critical) {
            tray->showMessage(title,
                              msg,
                              QSystemTrayIcon::Critical,
                              900000);
        } else {
            tray->showMessage(title, msg);
        }
    }
}

void App::handleConfChanged(const QString &file)
{
    Q_UNUSED(file)
    loadSettings();
}

void App::disableHibernate()
{
    if (criticalAction == criticalHibernate) {
        qWarning() << "reset critical action to shutdown";
        criticalAction = criticalShutdown;
        Settings::setValue(CONF_CRITICAL_BATTERY_ACTION,
                           criticalAction);
    }
    if (lidActionBattery == lidHibernate) {
        qWarning() << "reset lid battery action to lock";
        lidActionBattery = lidLock;
        Settings::setValue(CONF_LID_BATTERY_ACTION,
                           lidActionBattery);
    }
    if (lidActionAC == lidHibernate) {
        qWarning() << "reset lid ac action to lock";
        lidActionAC = lidLock;
        Settings::setValue(CONF_LID_AC_ACTION,
                           lidActionAC);
    }
    if (autoSuspendBatteryAction == suspendHibernate) {
        qWarning() << "reset auto suspend battery action to none";
        autoSuspendBatteryAction = suspendNone;
        Settings::setValue(CONF_SUSPEND_BATTERY_ACTION,
                           autoSuspendBatteryAction);
    }
    if (autoSuspendACAction == suspendHibernate) {
        qWarning() << "reset auto suspend ac action to none";
        autoSuspendACAction = suspendNone;
        Settings::setValue(CONF_SUSPEND_AC_ACTION,
                           autoSuspendACAction);
    }
}

void App::disableSuspend()
{
    if (lidActionBattery == lidSleep) {
        qWarning() << "reset lid battery action to lock";
        lidActionBattery = lidLock;
        Settings::setValue(CONF_LID_BATTERY_ACTION,
                           lidActionBattery);
    }
    if (lidActionAC == lidSleep) {
        qWarning() << "reset lid ac action to lock";
        lidActionAC = lidLock;
        Settings::setValue(CONF_LID_AC_ACTION,
                           lidActionAC);
    }
    if (autoSuspendBatteryAction == suspendSleep) {
        qWarning() << "reset auto suspend battery action to none";
        autoSuspendBatteryAction = suspendNone;
        Settings::setValue(CONF_SUSPEND_BATTERY_ACTION,
                           autoSuspendBatteryAction);
    }
    if (autoSuspendACAction == suspendSleep) {
        qWarning() << "reset auto suspend ac action to none";
        autoSuspendACAction = suspendNone;
        Settings::setValue(CONF_SUSPEND_AC_ACTION,
                           autoSuspendACAction);
    }
}

void App::handlePrepareForSuspend()
{
    /*qDebug() << "prepare for suspend";
    resetTimer();
    man->ReleaseSuspendLock();*/
    qDebug() << "do nothing";
}

void App::handlePrepareForResume()
{
    qDebug() << "prepare for resume ...";
    resetTimer();
    ss->setDisplaysOff(false);
}

// turn off/on monitor using xrandr
void App::switchInternalMonitor(bool toggle)
{
    if (!lidXrandr) { return; }
    qDebug() << "using xrandr to turn on/off internal monitor" << internalMonitor << toggle;
    QStringList args;
    args << "--output" << internalMonitor << (toggle ? "--auto" : "--off");
    QProcess::startDetached("xrandr", args);
}

// adjust backlight on wheel event (on systray)
void App::handleTrayWheel(TrayIcon::WheelAction action)
{
    if (!backlightMouseWheel) { return; }
    switch (action) {
    case TrayIcon::WheelUp:
        Backlight::setBrightness(backlightDevice,
                                 Backlight::getCurrentBrightness(backlightDevice)+POWERKIT_BACKLIGHT_STEP);
        break;
    case TrayIcon::WheelDown:
        Backlight::setBrightness(backlightDevice,
                                 Backlight::getCurrentBrightness(backlightDevice)-POWERKIT_BACKLIGHT_STEP);
        break;
    default:;
    }
}

void App::handleDeviceChanged(const QString &path)
{
    Q_UNUSED(path)
    checkDevices();
}

void App::openSettings()
{
    QProcess::startDetached(qApp->applicationFilePath(),
                            QStringList() << "--config");
}

void App::handleError(const QString &message)
{
    qWarning() << "ERROR:" << message;
    showMessage(tr("Error"), message, true);
    QTimer::singleShot(5000, qApp, SLOT(quit()));
}

void App::handleWarning(const QString &message)
{
    qWarning() << "WARNING:" << message;
    showMessage(tr("Warning"), message, true);
}

bool TrayIcon::event(QEvent *e)
{
    if (e->type() == QEvent::Wheel) {
        QWheelEvent *w = (QWheelEvent*)e;
        //if (w->orientation() == Qt::Vertical) {
            wheel_delta += w->angleDelta().y();
            if (abs(wheel_delta) >= 120) {
                emit wheel(wheel_delta > 0 ? TrayIcon::WheelUp : TrayIcon::WheelDown);
                wheel_delta = 0;
            }
        //}
        return true;
    }
    return QSystemTrayIcon::event(e);
}
