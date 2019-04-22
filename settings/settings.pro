#
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#

QT += core gui dbus
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = powerkit-settings
TEMPLATE = app

SOURCES += main.cpp dialog.cpp ../app/theme.cpp
HEADERS += dialog.h ../app/theme.h

LIBS += -L../lib -lPowerKit
INCLUDEPATH += ../lib ../app

include(../powerkit.pri)
DEFINES += APP_VERSION=\"\\\"$${VERSION}\\\"\"
DEFINES += APP_VERSION_EXTRA=\"\\\"$${VERSION_EXTRA}\\\"\"

!CONFIG(no_settings_install) {
    target.path = $${PREFIX}/bin
    INSTALLS += target
    !CONFIG(no_desktop_install) {
        target_desktop.path = $${PREFIX}/share/applications
        target_desktop.files = $${TARGET}.desktop
        INSTALLS += target_desktop
    }
}
