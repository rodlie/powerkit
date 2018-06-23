#
# powerdwarf <https://github.com/rodlie/powerdwarf>
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

!CONFIG(no_app_install) {
    target.path = $${PREFIX}/bin
    INSTALLS += target
    !CONFIG(no_doc_install) {
        target_docs.path = $${DOCDIR}/$${TARGET}-$${VERSION}
        target_docs.files = ../LICENSE ../README.md
        INSTALLS += target_docs
    }
    !CONFIG(no_desktop_install) {
        target_desktop.path = $${PREFIX}/share/applications
        target_desktop.files = share/applications/$${TARGET}.desktop
        INSTALLS += target_desktop
    }
    !CONFIG(no_autostart_install) {
        target_desktop_xdg.path = $${XDGDIR}/autostart
        target_desktop_xdg.files = share/autostart/$${TARGET}.desktop
        INSTALLS += target_desktop_xdg
    }
    CONFIG(install_udev_rules) {
        target_udev.path = $${UDEVDIR}/rules.d
        target_udev.files = share/udev/90-backlight.rules
        INSTALLS += target_udev
    }
}
