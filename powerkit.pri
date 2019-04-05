#
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#

VERSION = 1.1.0
VERSION_EXTRA = "beta"

isEmpty(PREFIX) {
    PREFIX = /usr/local
    isEmpty(XDGDIR): XDGDIR = $${PREFIX}/etc/xdg
    isEmpty(UDEVDIR): UDEVDIR = $${PREFIX}/etc/udev
}
isEmpty(DOCDIR): DOCDIR = $$PREFIX/share/doc
isEmpty(MANDIR): MANDIR = $$PREFIX/share/man
isEmpty(XDGDIR): XDGDIR = /etc/xdg
isEmpty(UDEVDIR): UDEVDIR = /etc/udev
isEmpty(DBUS_CONF): DBUS_CONF = /etc
isEmpty(DBUS_SERVICE): DBUS_SERVICE = /usr/share
isEmpty(USER): USER = root
isEmpty(GROUP): GROUP = users

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
    CONFIG += staticlib
}

LIBS += -lX11 -lXss -lXrandr
