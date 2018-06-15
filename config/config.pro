#
# PowerDwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#
#

QT += core gui dbus
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = powerdwarf-config
TEMPLATE = app

SOURCES += main.cpp dialog.cpp
HEADERS += dialog.h
RESOURCES += ../powerdwarf.qrc

LIBS += -L../lib -lPowerDwarf
INCLUDEPATH += ../lib

include(../powerdwarf.pri)

target.path = $${PREFIX}/bin
target_desktop.path = $${PREFIX}/share/applications
target_desktop.files = powerdwarf.desktop
INSTALLS += target target_desktop
