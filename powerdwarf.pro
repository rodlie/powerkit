#
# powerdwarf <https://github.com/rodlie/powerdwarf>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#

TEMPLATE = subdirs
CONFIG -= ordered
SUBDIRS += lib app
app.depends += lib

OTHER_FILES += \
            app/share/slackware/powerdwarf.info \
            app/share/slackware/powerdwarf.SlackBuild
