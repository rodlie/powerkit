#ifndef POWERDWARF_SERVICE_H
#define POWERDWARF_SERVICE_H

#include <QObject>

#define PD_SERVICE "org.freedesktop.PowerDwarf"
#define PD_PATH "/PowerDwarf"

class PowerDwarf : public QObject
{
    Q_OBJECT

public:
    explicit PowerDwarf(){}

signals:
    void updatedMonitors();
    void update();

private slots:
    void updateMonitors()
    {
        emit updatedMonitors();
    }

public slots:
    void refresh()
    {
        emit update();
    }
};

#endif // POWERDWARF_SERVICE_H
