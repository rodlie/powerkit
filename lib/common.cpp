/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "common.h"
#include <QFile>
#include <QFileInfo>
//#include <QIcon>
//#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QDebug>
#include <QDirIterator>
#include <QTextStream>

#include "def.h"

#define PK "powerkit"

void Common::savePowerSettings(QString type, QVariant value)
{
    QSettings settings(PK, PK);
    settings.setValue(type, value);
    settings.sync();
}

QVariant Common::loadPowerSettings(QString type)
{
    QSettings settings(PK, PK);
    return settings.value(type);
}

bool Common::validPowerSettings(QString type)
{
    QSettings settings(PK, PK);
    return settings.value(type).isValid();
}

void Common::saveDefaultSettings()
{
    savePowerSettings(CONF_LID_BATTERY_ACTION,
                      LID_BATTERY_DEFAULT);
    savePowerSettings(CONF_LID_AC_ACTION,
                      LID_AC_DEFAULT);
    savePowerSettings(CONF_CRITICAL_BATTERY_ACTION,
                      CRITICAL_DEFAULT);
    savePowerSettings(CONF_CRITICAL_BATTERY_TIMEOUT,
                      CRITICAL_BATTERY);
    savePowerSettings(CONF_SUSPEND_BATTERY_TIMEOUT,
                      AUTO_SLEEP_BATTERY);
    savePowerSettings(CONF_FREEDESKTOP_SS,
                      true);
    savePowerSettings(CONF_FREEDESKTOP_PM,
                      true);
    savePowerSettings(CONF_TRAY_NOTIFY,
                      true);
    savePowerSettings(CONF_TRAY_SHOW,
                      true);
    savePowerSettings(CONF_LID_DISABLE_IF_EXTERNAL,
                      false);
    savePowerSettings(CONF_SUSPEND_BATTERY_ACTION,
                      suspendSleep);
    savePowerSettings(CONF_SUSPEND_AC_ACTION,
                      suspendNone);
    savePowerSettings(CONF_BACKLIGHT_BATTERY_ENABLE,
                      false);
    savePowerSettings(CONF_BACKLIGHT_AC_ENABLE,
                      false);
    savePowerSettings(CONF_BACKLIGHT_BATTERY_DISABLE_IF_LOWER,
                      false);
    savePowerSettings(CONF_BACKLIGHT_AC_DISABLE_IF_HIGHER,
                      false);
    savePowerSettings(CONF_WARN_ON_LOW_BATTERY,
                      true);
    savePowerSettings(CONF_WARN_ON_VERYLOW_BATTERY,
                      true);
    savePowerSettings(CONF_NOTIFY_ON_BATTERY,
                      true);
    savePowerSettings(CONF_NOTIFY_ON_AC,
                      true);
    savePowerSettings(CONF_BACKLIGHT_MOUSE_WHEEL,
                      true);
    savePowerSettings(CONF_SUSPEND_LOCK_SCREEN,
                      true);
    savePowerSettings(CONF_RESUME_LOCK_SCREEN,
                      false);
}

/*void Common::setIconTheme()
{
    // setup icon theme search path
    QStringList iconsPath = QIcon::themeSearchPaths();
    QString iconsHomeLocal = QString("%1/.local/share/icons").arg(QDir::homePath());
    QString iconsHome = QString("%1/.icons").arg(QDir::homePath());
    if (QFile::exists(iconsHomeLocal) &&
        !iconsPath.contains(iconsHomeLocal)) { iconsPath.prepend(iconsHomeLocal); }
    if (QFile::exists(iconsHome) &&
        !iconsPath.contains(iconsHome)) { iconsPath.prepend(iconsHome); }
    iconsPath << QString("%1/../share/icons").arg(qApp->applicationDirPath());
    QIcon::setThemeSearchPaths(iconsPath);
    qDebug() << "using icon theme search path" << QIcon::themeSearchPaths();

    QString theme = QIcon::themeName();
    if (theme.isEmpty() || theme == "hicolor") { // try to load saved theme
        theme = loadPowerSettings(CONF_ICON_THEME).toString();
    }
    if(theme.isEmpty() || theme == "hicolor") { // Nope, then scan for first available
        // gtk
        if(QFile::exists(QDir::homePath() + "/" + ".gtkrc-2.0")) {
            QSettings gtkFile(QDir::homePath() + "/.gtkrc-2.0", QSettings::IniFormat);
            theme = gtkFile.value("gtk-icon-theme-name").toString().remove("\"");
        } else {
            QSettings gtkFile(QDir::homePath() + "/.config/gtk-3.0/settings.ini", QSettings::IniFormat);
            theme = gtkFile.value("gtk-fallback-icon-theme").toString().remove("\"");
        }
        // fallback
        if(theme.isNull()) { theme = DEFAULT_THEME; }
        if (!theme.isEmpty()) { savePowerSettings(CONF_ICON_THEME, theme); }
    }
    qDebug() << "Using icon theme" << theme;
    QIcon::setThemeName(theme);
#ifdef BUNDLE_ICONS
    if (theme != DEFAULT_THEME) { // validate theme
        QIcon testTheme = QIcon::fromTheme(DEFAULT_AC_ICON);
        if (testTheme.isNull()) {
            qDebug() << "icon theme is broken, use failsafe!";
            QIcon::setThemeName(DEFAULT_THEME);
            savePowerSettings(CONF_ICON_THEME, DEFAULT_THEME);
        }
    }
#endif
}*/

QString Common::confFile()
{
    QString config = QString("%1/.config/powerkit/powerkit.conf")
                     .arg(QDir::homePath());
    if (!QFile::exists(config)) { saveDefaultSettings(); }
    return config;
}

QString Common::confDir()
{
    QString config = QString("%1/.config/powerkit").arg(QDir::homePath());
    if (!QFile::exists(config)) {
        QDir dir(config);
        dir.mkpath(config);
    }
    return config;
}

bool Common::kernelCanResume(bool ignore)
{
    if (ignore) { return true; }
#ifdef __FreeBSD__
    // ???
    return false;
#endif
    QFile cmdline("/proc/cmdline");
    if (cmdline.open(QIODevice::ReadOnly)) {
        QByteArray result = cmdline.readAll();
        cmdline.close();
        if (result.contains("resume=")) { return true;}
    }
    return false;
}

QString Common::backlightDevice()
{
#ifdef __FreeBSD__
    // ???
    return QString();
#else
    QString path = "/sys/class/backlight";
    QDirIterator it(path, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString foundDir = it.next();
        if (foundDir.startsWith(QString("%1/radeon").arg(path))) {
            return foundDir;
        } else if (foundDir.startsWith(QString("%1/intel").arg(path))) {
            return foundDir;
        } else if (foundDir.startsWith(QString("%1/acpi").arg(path))) {
            return foundDir;
        }
    }
    return QString();
#endif
}

bool Common::canAdjustBacklight(QString device)
{
    QFileInfo backlight(QString("%1/brightness").arg(device));
    if (backlight.isWritable()) { return true; }
    return false;
}

int Common::backlightMax(QString device)
{
    int result = 0;
    QFile backlight(QString("%1/max_brightness").arg(device));
    if (backlight.open(QIODevice::ReadOnly)) {
        result = backlight.readAll().trimmed().toInt();
        backlight.close();
    }
    return result;
}

int Common::backlightValue(QString device)
{
    int result = 0;
    QFile backlight(QString("%1/brightness").arg(device));
    if (backlight.open(QIODevice::ReadOnly)) {
        result = backlight.readAll().trimmed().toInt();
        backlight.close();
    }
    return result;
}

bool Common::adjustBacklight(QString device, int value)
{
    if (!canAdjustBacklight(device)) { return false; }
    QFile backlight(QString("%1/brightness").arg(device));
    if (backlight.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        QTextStream out(&backlight);
        if (value<1) { value = 1; }
        out << QString::number(value);
        backlight.close();
        if (value == backlightValue(device)) { return true;}
    }
    return false;
}

void Common::checkSettings()
{
    confFile();
}

int Common::getCpuTotal()
{
#ifdef Q_OS_LINUX
    int counter = 0;
    while(1) {
        if (QFile::exists(QString("%1/cpu%2")
                          .arg(LINUX_CPU_SYS)
                          .arg(counter))) { counter++; }
        else { break; }
    }
    if (counter>0) { return counter; }
#endif
    return -1;
}

const QString Common::getCpuGovernor(int cpu)
{
    QString result;
#ifdef Q_OS_LINUX
    QFile gov(QString("%1/cpu%2/%3/%4")
              .arg(LINUX_CPU_SYS)
              .arg(cpu)
              .arg(LINUX_CPU_DIR)
              .arg(LINUX_CPU_GOVERNOR));
    if (!gov.exists()) { return result; }
    if (gov.open(QIODevice::ReadOnly|QIODevice::Text)) {
        result = gov.readAll().trimmed();
        gov.close();
    }
#else
    Q_UNUSED(cpu)
#endif
    return result;
}

const QStringList Common::getCpuGovernors()
{
    QStringList result;
    for (int i=0;i<getCpuTotal();++i) {
        QString value = getCpuGovernor(i);
        if (!value.isEmpty()) { result << value; }
    }
    return result;
}

const QStringList Common::getAvailableGovernors()
{
    QStringList result;
#ifdef Q_OS_LINUX
    QFile gov(QString("%1/cpu%2/%3/%4")
              .arg(LINUX_CPU_SYS)
              .arg(0)
              .arg(LINUX_CPU_DIR)
              .arg(LINUX_CPU_GOVERNORS));
    if (!gov.exists()) { return result; }
    if (gov.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString value = gov.readAll().trimmed();
        result = value.split(" ", QString::SkipEmptyParts);
        gov.close();
    }
#endif
    return result;
}

bool Common::cpuGovernorExists(const QString &gov)
{
    if (gov.isEmpty()) { return false; }
    return getAvailableGovernors().contains(gov);
}

bool Common::setCpuGovernor(const QString &gov, int cpu)
{
#ifdef Q_OS_LINUX
    if (!cpuGovernorExists(gov)) { return false; }
    QFile file(QString("%1/cpu%2/%3/%4")
              .arg(LINUX_CPU_SYS)
              .arg(cpu)
              .arg(LINUX_CPU_DIR)
              .arg(LINUX_CPU_GOVERNOR));
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        QTextStream out(&file);
        out << gov;
        file.close();
        if (gov == getCpuGovernor(cpu)) { return true;}
    }
#elif
    Q_UNUSED(gov)
    Q_UNUSED(cpu)
#endif
    return false;
}

bool Common::setCpuGovernor(const QString &gov)
{
    if (!cpuGovernorExists(gov)) { return false; }
    bool failed = false;
    for (int i=0;i<getCpuTotal();++i) {
        if (!setCpuGovernor(gov, i)) { failed = true; }
    }
    if (failed) { return false; }
    return true;
}

const QString Common::getCpuFrequency(int cpu)
{
    QString result;
#ifdef Q_OS_LINUX
    QFile freq(QString("%1/cpu%2/%3/%4")
              .arg(LINUX_CPU_SYS)
              .arg(cpu)
              .arg(LINUX_CPU_DIR)
              .arg(LINUX_CPU_FREQUENCY));
    if (!freq.exists()) { return result; }
    if (freq.open(QIODevice::ReadOnly|QIODevice::Text)) {
        result = freq.readAll().trimmed();
        freq.close();
    }
#else
    Q_UNUSED(cpu)
#endif
    return result;
}

const QStringList Common::getCpuFrequencies()
{
    QStringList result;
    for (int i=0;i<getCpuTotal();++i) {
        QString value = getCpuFrequency(i);
        if (!value.isEmpty()) { result << value; }
    }
    return result;
}

const QStringList Common::getCpuAvailableFrequency()
{
    QStringList result;
#ifdef Q_OS_LINUX
    QFile gov(QString("%1/cpu%2/%3/%4")
              .arg(LINUX_CPU_SYS)
              .arg(0)
              .arg(LINUX_CPU_DIR)
              .arg(LINUX_CPU_FREQUENCIES));
    if (!gov.exists()) { return result; }
    if (gov.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString value = gov.readAll().trimmed();
        result = value.split(" ", QString::SkipEmptyParts);
        gov.close();
    }
#endif
    return result;
}

bool Common::cpuFrequencyExists(const QString &freq)
{
    if (freq.isEmpty()) { return false; }
    return getCpuAvailableFrequency().contains(freq);
}

bool Common::setCpuFrequency(const QString &freq, int cpu)
{
#ifdef Q_OS_LINUX
    if (!cpuFrequencyExists(freq)) { return false; }
    if (getCpuGovernor(cpu) != "userspace") {
        if (!setCpuGovernor("userspace", cpu)) { return false; }
    }
    QFile file(QString("%1/cpu%2/%3/%4")
              .arg(LINUX_CPU_SYS)
              .arg(cpu)
              .arg(LINUX_CPU_DIR)
              .arg(LINUX_CPU_SET_SPEED));
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        QTextStream out(&file);
        out << freq;
        file.close();
        if (freq == getCpuFrequency(cpu)) { return true;}
    }
#elif
    Q_UNUSED(freq)
    Q_UNUSED(cpu)
#endif
    return false;
}

bool Common::setCpuFrequency(const QString &freq)
{
    if (!cpuFrequencyExists(freq)) { return false; }
    bool failed = false;
    for (int i=0;i<getCpuTotal();++i) {
        if (!setCpuFrequency(freq, i)) { failed = true; }
    }
    if (failed) { return false; }
    return true;
}

double Common::getBatteryLeft(QDBusInterface *iface)
{
    qDebug() << "check for battery left";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("BatteryLeft");
    double ok = reply.arguments().last().toDouble();
    qDebug() << "we have battery left" << ok;
    return  ok;
}

bool Common::hasBattery(QDBusInterface *iface)
{
    qDebug() << "check if we have any battery";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("HasBattery");
    bool ok = reply.arguments().last().toBool();
    qDebug() << "we have any battery?" << ok;
    return  ok;
}

bool Common::onBattery(QDBusInterface *iface)
{
    qDebug() << "check if we are on battery";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("OnBattery");
    bool ok = reply.arguments().last().toBool();
    qDebug() << "we are on battery?" << ok;
    return  ok;
}

qlonglong Common::timeToEmpty(QDBusInterface *iface)
{
    qDebug() << "check for time to empty";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("TimeToEmpty");
    qlonglong ok = reply.arguments().last().toLongLong();
    qDebug() << "we have time to empty?" << ok;
    return  ok;
}

bool Common::canHibernate(QDBusInterface *iface)
{
    qDebug() << "check if we can hibernate";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("CanHibernate");
    bool ok = reply.arguments().last().toBool();
    qDebug() << "we can hibernate?" << ok;
    return  ok;
}

bool Common::canSuspend(QDBusInterface *iface)
{
    qDebug() << "check if we can suspend";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("CanSuspend");
    bool ok = reply.arguments().last().toBool();
    qDebug() << "we can suspend?" << ok;
    return  ok;
}

bool Common::lidIsPresent(QDBusInterface *iface)
{
    qDebug() << "check if we have a lid";
    if (!iface->isValid()) { return false; }
    QDBusMessage reply = iface->call("LidIsPresent");
    bool ok = reply.arguments().last().toBool();
    qDebug() << "we have a lid?" << ok;
    return  ok;
}
