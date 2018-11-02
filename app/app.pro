#
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#

QT += core gui dbus
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = powerkit
TEMPLATE = app

SOURCES += main.cpp systray.cpp dialog.cpp common.cpp
HEADERS += systray.h dialog.h common.h

LIBS += -L../lib -lPowerKit
INCLUDEPATH += ../lib

include(../powerkit.pri)
DEFINES += APP_VERSION=\"\\\"$${VERSION}\\\"\"
DEFINES += APP_VERSION_EXTRA=\"\\\"$${VERSION_EXTRA}\\\"\"

CONFIG(bundle_icons) {
    RESOURCES += icons.qrc
    DEFINES += BUNDLE_ICONS
}
!CONFIG(no_app_install) {
    target.path = $${PREFIX}/bin
    INSTALLS += target
    !CONFIG(no_doc_install) {
        target_docs.path = $${DOCDIR}/$${TARGET}-$${VERSION}$${VERSION_EXTRA}
        target_docs.files = ../LICENSE ../README.md ../ChangeLog
        CONFIG(bundle_icons): target_docs.files += icons/Adwaita/LICENSE.Adwaita
        exists(../ChangeLog.git): target_docs.files += ../ChangeLog.git
        INSTALLS += target_docs
    }
    !CONFIG(no_man_install) {
        target_man.path = $${MANDIR}/man1
        target_man.files = share/man/$${TARGET}.1
        INSTALLS += target_man
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
