/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkitd.h"

#include <QDebug>
#include <QFile>
#include <QLocalSocket>

#include <unistd.h> // chown

PowerKitD::PowerKitD(QObject *parent) : QObject(parent)
  , _ipc(nullptr)
{
    qDebug("started powerkitd");
    if (!startServer()) {
        qWarning("failed to start ipc server!");
    }
}

PowerKitD::~PowerKitD()
{
    stopServer();
    qDebug("ended powerkitd");
}

QVector<QPair<int, QString> > PowerKitD::getSystemGroups()
{
    qDebug("get system groups info");
    QVector<QPair<int, QString> > result;
    if (QFile::exists(ETC_GROUP)) {
        QString txt;
        QFile file(ETC_GROUP);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            txt = file.readAll();
            file.close();
        }
        QStringList lines = txt.split("\n");
        for (int i=0;i<lines.size();++i) {
            QString line = lines.at(i);
            QStringList values = line.split(":");
            if (values.size()>=3) {
                QPair<int, QString> group;
                group.second = values.at(0);
                group.first = QString(values.at(2)).toInt();
                result.append(group);
            }
        }
    }
    return result;
}

int PowerKitD::getSystemGroupID(const QString &group)
{
    qDebug("get system group id");
    QVector<QPair<int, QString> > result = getSystemGroups();
    for (int i=0;i<result.size();++i) {
        if (result.at(i).second == group) { return result.at(i).first; }
    }
    return -1;
}

bool PowerKitD::setFileAccess(const QString &path, int user, int group)
{
    qDebug("set file access");
    if (!QFile::exists(path) || user <0 || group <0) { return false; }
    int res = chown(path.toUtf8(), user, group);
    if (res == 0) { return true; }
    return false;
}

bool PowerKitD::startServer()
{
    qDebug("start socket server");
    int group = getSystemGroupID(POWERKITD_GROUP);
    if (group<0) { return false; }
    if (!_ipc) {
        _ipc = new QLocalServer(this);
        connect(_ipc, &QLocalServer::newConnection,
                this, &PowerKitD::handleNewConnection);
    }
    QLocalServer::removeServer(POWERKITD_HOST);
    if (_ipc->listen(POWERKITD_HOST)) {
        /*if (setFileAccess(_ipc->fullServerName(), 0, group)) {
            return true;
        }*/
        return true;
    }
    return false;
}

void PowerKitD::stopServer()
{
    qDebug("stop socket server");
    if (_ipc) {
        if (_ipc->isListening()) { _ipc->close(); }
        _ipc->disconnect();
    }
    QLocalServer::removeServer(POWERKITD_HOST);
}

void PowerKitD::handleNewConnection()
{
    qDebug("handle new socket connection");
    QLocalSocket *socket = _ipc->nextPendingConnection();
    connect(socket, &QLocalSocket::disconnected,
            socket, &QLocalSocket::deleteLater);
    connect(socket, &QLocalSocket::readyRead,
            this, &PowerKitD::readNewConnection);
}

void PowerKitD::readNewConnection()
{
    qDebug("read new socket connection");
    QLocalSocket* socket = qobject_cast<QLocalSocket*>(sender());
    if (!socket) { return; }
    QByteArray message = socket->readAll();
    qDebug() << message;

    // return to sender
    socket->write(message);
    socket->flush();
    socket->disconnectFromServer();
}
