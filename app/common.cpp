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

void Common::setIconTheme()
{
    // setup icon theme search path
    QStringList iconsPath = QIcon::themeSearchPaths();
    QString iconsHomeLocal = QString("%1/.local/share/icons").arg(QDir::homePath());
    QString iconsHome = QString("%1/.icons").arg(QDir::homePath());
    if (QFile::exists(iconsHomeLocal) && !iconsPath.contains(iconsHomeLocal)) { iconsPath.prepend(iconsHomeLocal); }
    if (QFile::exists(iconsHome) && !iconsPath.contains(iconsHome)) { iconsPath.prepend(iconsHome); }
    iconsPath << QString("%1/../share/icons").arg(qApp->applicationDirPath());
    QIcon::setThemeSearchPaths(iconsPath);
    qDebug() << "using icon theme search path" << QIcon::themeSearchPaths();

    QString theme = QIcon::themeName();
    if (theme.isEmpty() || theme == "hicolor") { // try to load saved theme
        qDebug() << "checking for icon theme in settings";
        theme = loadPowerSettings("icon_theme").toString();
    }
    if(theme.isEmpty() || theme == "hicolor") { // Nope, then scan for first available
        // gtk
        if(QFile::exists(QDir::homePath() + "/" + ".gtkrc-2.0")) {
            qDebug() << "checking for icon theme in gtkrc-2.0";
            QSettings gtkFile(QDir::homePath() + "/.gtkrc-2.0", QSettings::IniFormat);
            theme = gtkFile.value("gtk-icon-theme-name").toString().remove("\"");
        } else {
            qDebug() << "checking for icon theme in gtk-3.0";
            QSettings gtkFile(QDir::homePath() + "/.config/gtk-3.0/settings.ini", QSettings::IniFormat);
            theme = gtkFile.value("gtk-fallback-icon-theme").toString().remove("\"");
        }
        // fallback
        if(theme.isNull()) {
            qDebug() << "checking for icon theme in static fallback";
            QStringList themes;
            themes << QString("%1/../share/icons/Humanity").arg(qApp->applicationFilePath());
            themes << "/usr/share/icons/Humanity" << "/usr/local/share/icons/Humanity";
            themes << QString("%1/../share/icons/Adwaita").arg(qApp->applicationFilePath());
            themes << "/usr/share/icons/Adwaita" << "/usr/local/share/icons/Adwaita";
            themes << QString("%1/../share/icons/gnome").arg(qApp->applicationFilePath());
            themes << "/usr/share/icons/gnome" << "/usr/local/share/icons/gnome";
            themes << QString("%1/../share/icons/oxygen").arg(qApp->applicationFilePath());
            themes << "/usr/share/icons/oxygen" << "/usr/local/share/icons/oxygen";
            themes << QString("%1/../share/icons/hicolor").arg(qApp->applicationFilePath());
            themes << QString("%1/../share/icons/Tango").arg(qApp->applicationFilePath());
            themes << "/usr/share/icons/Tango" << "/usr/local/share/icons/Tango";
            themes << "/usr/share/icons/hicolor" << "/usr/local/share/icons/hicolor";
            for (int i=0;i<themes.size();++i) {
                if (QFile::exists(themes.at(i))) {
                    theme = QString(themes.at(i)).split("/").takeLast();
                    break;
                }
            }
        }
        if (theme != "hicolor" && !theme.isEmpty()) {
            qDebug() << "save icon theme for later use";
            savePowerSettings("icon_theme", theme);
        } else {
            qDebug() << "No icons available!";
        }
    }
    QIcon::setThemeName(theme);
}

QString Common::confFile()
{
    QString config = QString("%1/.config/powerkit/powerkit.conf")
                     .arg(QDir::homePath());
    if (!QFile::exists(config)) {
        QFile conf(config);
        if (conf.open(QIODevice::WriteOnly)) { conf.close(); }
    }
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
