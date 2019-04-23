/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include <QApplication>
#include "powerkit_dialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog dialog;
    dialog.show();
    return a.exec();
}
