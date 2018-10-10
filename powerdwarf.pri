#
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#

VERSION = 1.0.0
VERSION_EXTRA = "beta8"

isEmpty(PREFIX) {
    PREFIX = /usr/local
    isEmpty(XDGDIR) {
        XDGDIR = $${PREFIX}/etc/xdg
    }
    isEmpty(UDEVDIR) {
        UDEVDIR = $${PREFIX}/etc/udev
    }
}
isEmpty(DOCDIR) {
    DOCDIR = $$PREFIX/share/doc
}
isEmpty(MANDIR) {
    MANDIR = $$PREFIX/share/man
}
isEmpty(XDGDIR) {
    XDGDIR = /etc/xdg
}
isEmpty(UDEVDIR) {
    UDEVDIR = /etc/udev
}

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
    CONFIG += staticlib
}

freebsd {
    INCLUDEPATH += /usr/local/include
    LIBS += -lX11 -lXss -lXrandr
} else {
    CONFIG += link_pkgconfig
    PKGCONFIG += x11 xscrnsaver xrandr
}
