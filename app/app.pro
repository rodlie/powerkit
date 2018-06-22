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

TARGET = powerdwarf
TEMPLATE = app

SOURCES += main.cpp systray.cpp dialog.cpp
HEADERS += systray.h dialog.h
RESOURCES += ../$${TARGET}.qrc
LIBS += -L../lib -lPowerDwarf
INCLUDEPATH += ../lib

include(../powerdwarf.pri)

target.path = $${PREFIX}/bin
target_desktop.path = $${PREFIX}/share/applications
target_desktop.files = share/applications/$${TARGET}.desktop
target_desktop_xdg.path = $${XDGDIR}/autostart
target_desktop_xdg.files = share/autostart/$${TARGET}.desktop
target_docs.path = $${DOCDIR}/$${TARGET}-$${VERSION}
target_docs.files = ../LICENSE ../README.md
INSTALLS += target target_desktop target_desktop_xdg target_docs
