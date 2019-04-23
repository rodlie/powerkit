#
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com>.
# All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#

QT += dbus
QT -= gui

TARGET = PowerKit
TEMPLATE = lib

SOURCES += \
    device.cpp \
    screens.cpp \
    powerkit.cpp \
    rtc.cpp \
    common.cpp \
    hotplug.cpp

HEADERS += \
    def.h \
    device.h \
    screens.h \
    powerkit.h \
    rtc.h \
    common.h \
    hotplug.h

HEADERS += \
    powerkit_backlight.h \
    powerkit_cpu.h \
    powerkit_settings.h \
    powerkit_client.h \
    powerkit_freedesktop_pm.h \
    powerkit_freedesktop_ss.h
SOURCES += \
    powerkit_backlight.cpp \
    powerkit_cpu.cpp \
    powerkit_settings.cpp \
    powerkit_client.cpp \
    powerkit_freedesktop_pm.cpp \
    powerkit_freedesktop_ss.cpp

include(../powerkit.pri)
CONFIG(install_lib) {
    CONFIG -= staticlib
    target.path = $${PREFIX}/lib$${LIBSUFFIX}
    INSTALLS += target
    !CONFIG(no_doc_install) {
        target_docs.path = $${DOCDIR}/powerkit-$${VERSION}$${VERSION_EXTRA}
        target_docs.files = ../LICENSE ../README.md ../ChangeLog
        exists(../ChangeLog.git): target_docs.files += ../ChangeLog.git
        INSTALLS += target_docs
    }
    !CONFIG(no_include_install) {
        target_inc.path = $${PREFIX}/include/powerkit
        target_inc.files = $${HEADERS}
        INSTALLS += target_inc
    }
    !CONFIG(no_pkgconfig_install) {
        CONFIG += create_prl no_install_prl create_pc
        QMAKE_PKGCONFIG_NAME = $${TARGET}
        QMAKE_PKGCONFIG_DESCRIPTION = PowerKit
        QMAKE_PKGCONFIG_LIBDIR = $$target.path
        QMAKE_PKGCONFIG_INCDIR = $$target_inc.path
        QMAKE_PKGCONFIG_DESTDIR = pkgconfig
    }
}
