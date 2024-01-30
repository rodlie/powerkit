/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWERKIT_THEME_H
#define POWERKIT_THEME_H

#include <QPixmap>
#include <QColor>

namespace PowerKit
{
    class Theme
    {
    public:
        static void setAppTheme();
        static void setIconTheme();
        static const QPixmap drawCircleProgress(const int &progress,
                                                const int &dimension,
                                                const int &width,
                                                const int &padding,
                                                const bool dash,
                                                const QString &text,
                                                const QColor &color1 = Qt::red,
                                                const QColor &color2 = Qt::white);
    };
}

#endif // POWERKIT_THEME_H
