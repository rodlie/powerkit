/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
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
#include <QPalette>
#include <QDebug>
#include <QPainter>
#include <QPen>

#include "powerkit_def.h"
#include "powerkit_settings.h"

void Theme::setAppTheme()
{
    bool native = PowerSettings::getValue("native_theme", false).toBool();
    if (native) { return; }
    qApp->setStyle("Fusion");
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53,53,53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(15,15,15));
    palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    palette.setColor(QPalette::Link, Qt::white);
    palette.setColor(QPalette::LinkVisited, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53,53,53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Highlight, QColor(196,110,33));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    qApp->setPalette(palette);
}

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

const QPixmap Theme::drawCircleProgress(const int &progress,
                                        const int &dimension,
                                        const int &width,
                                        const int &padding,
                                        const bool dash,
                                        const QString &text,
                                        const QColor &color1,
                                        const QColor &color2)
{
    double value = (double)progress / 100;
    if (value < 0.) { value = 0.; }
    else if (value > 1.) { value = 1.; }

    QRectF circle(padding / 2,
                  padding / 2,
                  dimension - padding,
                  dimension - padding);

    int startAngle = -90 * 16;
    int spanAngle = value * 360 * 16;

    QPixmap pix(dimension, dimension);
    pix.fill(Qt::transparent);

    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    QPen pen;
    pen.setWidth(width);
    pen.setCapStyle(Qt::FlatCap);
    pen.setColor(color1);
    if (dash) { pen.setDashPattern(QVector<qreal>{0.5, 1.105}); }

    QPen pen2;
    pen2.setWidth(width);
    pen2.setColor(color2);
    pen2.setCapStyle(Qt::FlatCap);

    p.setPen(pen);
    p.drawArc(circle, startAngle, 360 * 16);

    p.setPen(pen2);
    p.drawArc(circle, startAngle, spanAngle);

    if (!text.isEmpty()) {
        int textPadding = padding * 4;
        p.drawText(QRectF(textPadding / 2,
                          textPadding / 2,
                          dimension - textPadding,
                          dimension - textPadding),
                   Qt::AlignCenter,
                   text);
    }

    return pix;
}
