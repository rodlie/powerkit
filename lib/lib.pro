#
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#

QT += dbus gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PowerDwarf
TEMPLATE = lib
SOURCES += \
    common.cpp \
    power.cpp \
    upower.cpp \
    powermanagement.cpp \
    screensaver.cpp \
    login1.cpp \
    device.cpp \
    ckit.cpp \
    screens.cpp
HEADERS += \
    common.h \
    power.h \
    upower.h \
    powermanagement.h \
    screensaver.h \
    def.h \
    service.h \
    login1.h \
    device.h \
    ckit.h \
    screens.h

include(../powerdwarf.pri)

CONFIG(install_lib) {
    CONFIG -= staticlib
    target.path = $${PREFIX}/lib$${LIBSUFFIX}
    INSTALLS += target
    !CONFIG(no_doc_install) {
        target_docs.path = $${DOCDIR}/powerdwarf-$${VERSION}$${VERSION_EXTRA}
        target_docs.files = ../LICENSE ../README.md ../ChangeLog
        INSTALLS += target_docs
    }
    !CONFIG(no_include_install) {
        target_inc.path = $${PREFIX}/include/powerdwarf
        target_inc.files = $${HEADERS}
        INSTALLS += target_inc
    }
    !CONFIG(no_pkgconfig_install) {
        CONFIG += create_prl no_install_prl create_pc
        QMAKE_PKGCONFIG_NAME = $${TARGET}
        QMAKE_PKGCONFIG_DESCRIPTION = PowerDwarf
        QMAKE_PKGCONFIG_LIBDIR = $$target.path
        QMAKE_PKGCONFIG_INCDIR = $$target_inc.path
        QMAKE_PKGCONFIG_DESTDIR = pkgconfig
    }
}
