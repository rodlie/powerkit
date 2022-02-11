/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018-2022 Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit_theme.h"
#include <QFile>
#include <QIcon>
#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QDebug>

#include "powerkit_def.h"
#include "powerkit_settings.h"

void Theme::setIconTheme()
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
        theme = PowerSettings::getValue(CONF_ICON_THEME).toString();
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
        if (!theme.isEmpty()) { PowerSettings::setValue(CONF_ICON_THEME, theme); }
    }
    qDebug() << "Using icon theme" << theme;
    QIcon::setThemeName(theme);
#ifdef BUNDLE_ICONS
    if (theme != DEFAULT_THEME) { // validate theme
        QIcon testTheme = QIcon::fromTheme(DEFAULT_AC_ICON);
        if (testTheme.isNull()) {
            qDebug() << "icon theme is broken, use failsafe!";
            QIcon::setThemeName(DEFAULT_THEME);
            Common::savePowerSettings(CONF_ICON_THEME, DEFAULT_THEME);
        }
    }
#endif
}
