/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include <QCoreApplication>

#include "powerkitd.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    PowerKitD powerkitd;
    return a.exec();
}
