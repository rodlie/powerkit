#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#

isEmpty(PREFIX) {
    PREFIX = /usr/local
    isEmpty(XDGDIR) {
        XDGDIR = $${PREFIX}/etc/xdg
    }
}
isEmpty(DOCDIR) {
    DOCDIR = $$PREFIX/share/doc
}
isEmpty(XDGDIR) {
    XDGDIR = /etc/xdg
}

CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG_OUTPUT

#CONFIG -= install_lib
#CONFIG += staticlib

VERSION = 1.0.0
