/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "powerkit_cpu.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

int PowerCpu::getTotal()
{
    int counter = 0;
    while(1) {
        if (QFile::exists(QString("%1/cpu%2")
                          .arg(LINUX_CPU_SYS)
                          .arg(counter))) { counter++; }
        else { break; }
    }
    if (counter>0) { return counter; }
    return -1;
}

const QString PowerCpu::getGovernor(int cpu)
{
    QString result;
    QFile gov(QString("%1/cpu%2/%3/%4")
              .arg(LINUX_CPU_SYS)
              .arg(cpu)
              .arg(LINUX_CPU_DIR)
              .arg(LINUX_CPU_GOVERNOR));
    if (!gov.exists()) { return result; }
    if (gov.open(QIODevice::ReadOnly|QIODevice::Text)) {
        result = gov.readAll().trimmed();
        gov.close();
    }
    return result;
}

const QStringList PowerCpu::getGovernors()
{
    QStringList result;
    for (int i=0;i<getTotal();++i) {
        QString value = getGovernor(i);
        if (!value.isEmpty()) { result << value; }
    }
    return result;
}

const QStringList PowerCpu::getAvailableGovernors()
{
    QStringList result;
    QFile gov(QString("%1/cpu%2/%3/%4")
              .arg(LINUX_CPU_SYS)
              .arg(0)
              .arg(LINUX_CPU_DIR)
              .arg(LINUX_CPU_GOVERNORS));
    if (!gov.exists()) { return result; }
    if (gov.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString value = gov.readAll().trimmed();
        result = value.split(" ", QString::SkipEmptyParts);
        gov.close();
    }
    return result;
}

bool PowerCpu::governorExists(const QString &gov)
{
    if (gov.isEmpty()) { return false; }
    return getAvailableGovernors().contains(gov);
}

bool PowerCpu::setGovernor(const QString &gov, int cpu)
{
    if (!governorExists(gov)) { return false; }
    QFile file(QString("%1/cpu%2/%3/%4")
              .arg(LINUX_CPU_SYS)
              .arg(cpu)
              .arg(LINUX_CPU_DIR)
              .arg(LINUX_CPU_GOVERNOR));
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        QTextStream out(&file);
        out << gov;
        file.close();
        if (gov == getGovernor(cpu)) { return true;}
    }
    return false;
}

bool PowerCpu::setGovernor(const QString &gov)
{
    if (!governorExists(gov)) { return false; }
    bool failed = false;
    for (int i=0;i<getTotal();++i) {
        if (!setGovernor(gov, i)) { failed = true; }
    }
    if (failed) { return false; }
    return true;
}

const QString PowerCpu::getFrequency(int cpu)
{
    QString result;
    QFile freq(QString("%1/cpu%2/%3/%4")
              .arg(LINUX_CPU_SYS)
              .arg(cpu)
              .arg(LINUX_CPU_DIR)
              .arg(LINUX_CPU_FREQUENCY));
    if (!freq.exists()) { return result; }
    if (freq.open(QIODevice::ReadOnly|QIODevice::Text)) {
        result = freq.readAll().trimmed();
        freq.close();
    }
    return result;
}

const QStringList PowerCpu::getFrequencies()
{
    QStringList result;
    for (int i=0;i<getTotal();++i) {
        QString value = getFrequency(i);
        if (!value.isEmpty()) { result << value; }
    }
    return result;
}

const QStringList PowerCpu::getAvailableFrequency()
{
    QStringList result;
    QFile gov(QString("%1/cpu%2/%3/%4")
              .arg(LINUX_CPU_SYS)
              .arg(0)
              .arg(LINUX_CPU_DIR)
              .arg(LINUX_CPU_FREQUENCIES));
    if (!gov.exists()) { return result; }
    if (gov.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString value = gov.readAll().trimmed();
        result = value.split(" ", QString::SkipEmptyParts);
        gov.close();
    }
    return result;
}

bool PowerCpu::frequencyExists(const QString &freq)
{
    if (freq.isEmpty()) { return false; }
    return getAvailableFrequency().contains(freq);
}

bool PowerCpu::setFrequency(const QString &freq, int cpu)
{
    if (!frequencyExists(freq)) { return false; }
    if (getGovernor(cpu) != "userspace") {
        if (!setGovernor("userspace", cpu)) { return false; }
    }
    QFile file(QString("%1/cpu%2/%3/%4")
              .arg(LINUX_CPU_SYS)
              .arg(cpu)
              .arg(LINUX_CPU_DIR)
              .arg(LINUX_CPU_SET_SPEED));
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        QTextStream out(&file);
        out << freq;
        file.close();
        if (freq == getFrequency(cpu)) { return true;}
    }
    return false;
}

bool PowerCpu::setFrequency(const QString &freq)
{
    if (!frequencyExists(freq)) { return false; }
    bool failed = false;
    for (int i=0;i<getTotal();++i) {
        if (!setFrequency(freq, i)) { failed = true; }
    }
    if (failed) { return false; }
    return true;
}

bool PowerCpu::hasPState()
{
    return QFile::exists(QString("%1/%2")
                         .arg(LINUX_CPU_SYS)
                         .arg(LINUX_CPU_PSTATE));
}

bool PowerCpu::hasPStateTurbo()
{
    bool result = false;
    if (!hasPState()) { return result; }
    QFile file(QString("%1/%2/%3")
                .arg(LINUX_CPU_SYS)
                .arg(LINUX_CPU_PSTATE)
                .arg(LINUX_CPU_PSTATE_NOTURBO));
    if (!file.exists()) { return result; }
    if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString value = file.readAll().trimmed();
        file.close();
        if (value=="1") { result = false; }
        else if (value=="0") { result = true; }
    }
    return result;
}

bool PowerCpu::setPStateTurbo(bool turbo)
{
    if (!hasPState()) { return false; }
    QFile file(QString("%1/%2/%3")
              .arg(LINUX_CPU_SYS)
              .arg(LINUX_CPU_PSTATE)
              .arg(LINUX_CPU_PSTATE_NOTURBO));
    if (!file.exists()) { return false; }
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        QTextStream out(&file);
        if (turbo) { out << "0"; }
        else { out << "1"; }
        file.close();
        if (turbo == hasPStateTurbo()) { return true; }
    }
    return false;
}

int PowerCpu::getPStateMax()
{
    int value = -1;
    if (!hasPState()) { return value; }
    QFile file(QString("%1/%2/%3")
                .arg(LINUX_CPU_SYS)
                .arg(LINUX_CPU_PSTATE)
                .arg(LINUX_CPU_PSTATE_MAX_PERF));
    if (!file.exists()) { return value; }
    if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        value = file.readAll().trimmed().toInt();
        file.close();
    }
    return value;
}

int PowerCpu::getPStateMin()
{
    int value = -1;
    if (!hasPState()) { return value; }
    QFile file(QString("%1/%2/%3")
                .arg(LINUX_CPU_SYS)
                .arg(LINUX_CPU_PSTATE)
                .arg(LINUX_CPU_PSTATE_MIN_PERF));
    if (!file.exists()) { return value; }
    if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        value = file.readAll().trimmed().toInt();
        file.close();
    }
    return value;
}

bool PowerCpu::setPStateMax(int maxState)
{
    if (!hasPState()) { return false; }
    QFile file(QString("%1/%2/%3")
              .arg(LINUX_CPU_SYS)
              .arg(LINUX_CPU_PSTATE)
              .arg(LINUX_CPU_PSTATE_MAX_PERF));
    if (!file.exists()) { return false; }
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        QTextStream out(&file);
        out << QString::number(maxState);
        file.close();
        return true;
    }
    return false;
}

bool PowerCpu::setPStateMin(int minState)
{
    if (!hasPState()) { return false; }
    QFile file(QString("%1/%2/%3")
              .arg(LINUX_CPU_SYS)
              .arg(LINUX_CPU_PSTATE)
              .arg(LINUX_CPU_PSTATE_MIN_PERF));
    if (!file.exists()) { return false; }
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        QTextStream out(&file);
        out << QString::number(minState);
        file.close();
        return true;
    }
    return false;
}

bool PowerCpu::hasCoreTemp()
{
    return QFile::exists(QString(LINUX_CORETEMP).arg(0));
}

int PowerCpu::getCoreTemp()
{
    double temp = 0.0;
    if (!hasCoreTemp()) { return temp; }

    int temps = 1;
    int hwmons = 10;
    for (int hwmon = 0; hwmon < hwmons; ++hwmon) {
        bool foundPackage = false;
        QFile file(QString("%1/%2").arg(QString(LINUX_CORETEMP).arg(hwmon),
                                        QString(LINUX_CORETEMP_LABEL).arg(temps)));
        if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QString label = file.readAll().trimmed();
            if (label.startsWith("Package")) { foundPackage = true; }
            file.close();
        }
        if (foundPackage) {
            QFile file(QString("%1/%2").arg(QString(LINUX_CORETEMP).arg(hwmon),
                                            QString(LINUX_CORETEMP_INPUT).arg(temps)));
            if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
                double ctemp = file.readAll().trimmed().toDouble();
                file.close();
                if (ctemp > temp) {
                    temp = ctemp;
                    break;
                }
            }
        }
    }
    return temp;
}
