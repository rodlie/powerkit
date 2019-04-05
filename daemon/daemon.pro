#
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#

TARGET = powerkitd
QT += core dbus
QT -= gui
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp manager.cpp
HEADERS += manager.h
OTHER_FILES += $${TARGET}.conf.in

LIBS += -L../lib -lPowerKit
INCLUDEPATH += ../lib
include(../powerkit.pri)

target.path = $${PREFIX}/sbin
target_dbus_conf.path = $${DBUS_CONF}/dbus-1/system.d
target_dbus_service.path = $${DBUS_SERVICE}/dbus-1/system-services
target_dbus_conf.commands = $$quote(cat $${PWD}/$${TARGET}.conf.in | sed "\"s/_GROUP_/$${GROUP}/g;s/_USER_/$${USER}/g"\" > ${INSTALL_ROOT}$${DBUS_CONF}/dbus-1/system.d/org.freedesktop.$${TARGET}.conf$$escape_expand(\\n\\t))
target_dbus_service.commands = $$quote(echo "\"[D-BUS Service]"\" > ${INSTALL_ROOT}$${DBUS_SERVICE}/dbus-1/system-services/org.freedesktop.$${TARGET}.service$$escape_expand(\\n\\t))
target_dbus_service.commands += $$quote(echo "\"Name=org.freedesktop.$${TARGET}"\" >> ${INSTALL_ROOT}$${DBUS_SERVICE}/dbus-1/system-services/org.freedesktop.$${TARGET}.service$$escape_expand(\\n\\t))
target_dbus_service.commands += $$quote(echo "\"Exec=$${PREFIX}/sbin/$${TARGET}"\" >> ${INSTALL_ROOT}$${DBUS_SERVICE}/dbus-1/system-services/org.freedesktop.$${TARGET}.service$$escape_expand(\\n\\t))
target_dbus_service.commands += $$quote(echo "\"User=$${USER}"\" >> ${INSTALL_ROOT}$${DBUS_SERVICE}/dbus-1/system-services/org.freedesktop.$${TARGET}.service$$escape_expand(\\n\\t))
#target_docs.path = $${DOCDIR}/$${TARGET}-$${VERSION}
#target_docs.files = LICENSE README.md
#target_man.path = $${MANDIR}/man8
#target_man.files = $${TARGET}.8
INSTALLS += target target_dbus_conf target_dbus_service
# target_docs target_man

