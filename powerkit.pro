#
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#

TEMPLATE = subdirs
CONFIG -= ordered
SUBDIRS += lib app daemon settings
app.depends += lib
daemon.depends += lib
settings.depends += lib
