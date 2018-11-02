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
#include <QIcon>
#include <QApplication>
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

void Common::setIconTheme()
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
}

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

bool Common::kernelCanResume()
{
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
