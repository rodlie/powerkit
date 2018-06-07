#
# Power Dwarf <powerdwarf.dracolinux.org>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#

QT += dbus
QT -= gui
CONFIG += install_lib

TARGET = PowerDwarf
VERSION = 1.0.0

TEMPLATE = lib
SOURCES += power.cpp hotplug.cpp
HEADERS += power.h upower.h powermanagement.h screensaver.h common.h hotplug.h monitor.h

exists(../powerdwarf.pri) {
    include(../powerdwarf.pri)
}

CONFIG(install_lib) {
    CONFIG += create_prl no_install_prl create_pc
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    isEmpty(DOCDIR) {
        DOCDIR = $$PREFIX/share/doc
    }

    target.path = $${PREFIX}/lib$${LIBSUFFIX}
    target_docs.path = $${DOCDIR}/powerdwarf-$${VERSION}
    target_docs.files = ../LICENSE ../README.md
    target_inc.path = $${PREFIX}/include/powerdwarf
    target_inc.files = power.h powermanagement.h screensaver.h

    QMAKE_PKGCONFIG_NAME = $${TARGET}
    QMAKE_PKGCONFIG_DESCRIPTION = Power Dwarf Library
    QMAKE_PKGCONFIG_LIBDIR = $$target.path
    QMAKE_PKGCONFIG_INCDIR = $$target_inc.path
    QMAKE_PKGCONFIG_DESTDIR = pkgconfig

    INSTALLS += target target_docs target_inc
}
