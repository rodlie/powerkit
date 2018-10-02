/*
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef SCREENS_H
#define SCREENS_H

#include <QMap>
#include <QString>

#include "common.h"
#include <X11/extensions/Xrandr.h>

class Screens
{
public:
    static QMap<QString,bool> outputsDpy(Display *dpy);
    static QMap<QString,bool> outputs();
    static QString internalDpy(Display *dpy);
    static QString internal();
};

#endif // SCREENS_H
