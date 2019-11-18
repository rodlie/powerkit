/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWERKITD_H
#define POWERKITD_H

#include <QObject>
#include <QString>
#include <QLocalServer>
#include <QVector>
#include <QPair>

#define ETC_GROUP "/etc/group"
#define POWERKITD_HOST "powerkitd"
#define POWERKITD_GROUP "power"

class PowerKitD : public QObject
{
    Q_OBJECT

public:
    explicit PowerKitD(QObject *parent = nullptr);
    ~PowerKitD();

private:
    QLocalServer *_ipc;

signals:

private slots:
    /** @brief get available system groups and ids */
    static QVector<QPair<int, QString> > getSystemGroups();

    /** @brief get id for system group (-1 if none) */
    static int getSystemGroupID(const QString &group);

    /** @brief chown a path */
    static bool setFileAccess(const QString &path, int user, int group);

    /** @brief start ipc server */
    bool startServer();

    /** @brief stop ipc server */
    void stopServer();

    /** @brief handle new connection */
    void handleNewConnection();

    /** @brief read data from new connection */
    void readNewConnection();

public slots:
};

#endif // POWERKITD_H
