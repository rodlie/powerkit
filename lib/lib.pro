#
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#

QT += dbus xml
QT -= gui

TARGET = PowerKit
TEMPLATE = lib
SOURCES += \
    powermanagement.cpp \
    screensaver.cpp \
    device.cpp \
    screens.cpp \
    powerkit.cpp
HEADERS += \
    powermanagement.h \
    screensaver.h \
    def.h \
    device.h \
    screens.h \
    powerkit.h

include(../powerkit.pri)
CONFIG(install_lib) {
    CONFIG -= staticlib
    target.path = $${PREFIX}/lib$${LIBSUFFIX}
    INSTALLS += target
    !CONFIG(no_doc_install) {
        target_docs.path = $${DOCDIR}/powerkit-$${VERSION}$${VERSION_EXTRA}
        target_docs.files = ../LICENSE ../README.md ../ChangeLog
        INSTALLS += target_docs
    }
    !CONFIG(no_include_install) {
        target_inc.path = $${PREFIX}/include/powerkit
        target_inc.files = common.h \
                           powermanagement.h \
                           screensaver.h \
                           device.h \
                           screens.h
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
