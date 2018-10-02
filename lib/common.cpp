/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "common.h"
#include <QFile>
#include <QIcon>
#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QDebug>

#include "def.h"

void Common::savePowerSettings(QString type, QVariant value)
{
    QSettings settings(PD, PD);
    settings.setValue(type, value);
    settings.sync();
}

QVariant Common::loadPowerSettings(QString type)
{
    QSettings settings(PD, PD);
    return settings.value(type);
}

bool Common::validPowerSettings(QString type)
{
    QSettings settings(PD, PD);
    return settings.value(type).isValid();
}

void Common::setIconTheme()
{
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
            // TODO: home folder
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
    return QString("%1/.config/powerdwarf/powerdwarf.conf").arg(QDir::homePath());
}

QString Common::confDir()
{
    return QString("%1/.config/powerdwarf").arg(QDir::homePath());
}

bool Common::kernelCanResume()
{
#ifdef __FreeBSD__
    // ???
    return true;
#endif
    QFile cmdline("/proc/cmdline");
    if (cmdline.open(QIODevice::ReadOnly)) {
        QByteArray result = cmdline.readAll();
        cmdline.close();
        if (result.contains("resume=")) { return true;}
    }
    return false;
}
