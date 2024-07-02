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

#define LINUX_CPU_SYS "/sys/devices/system/cpu"
#define LINUX_CPU_DIR "cpufreq"
#define LINUX_CPU_FREQUENCIES "scaling_available_frequencies"
#define LINUX_CPU_FREQUENCY "scaling_cur_freq"
#define LINUX_CPU_FREQUENCY_MAX "scaling_max_freq"
#define LINUX_CPU_FREQUENCY_MIN "scaling_min_freq"
#define LINUX_CPU_GOVERNORS "scaling_available_governors"
#define LINUX_CPU_GOVERNOR "scaling_governor"
#define LINUX_CPU_SET_SPEED "scaling_setspeed"
#define LINUX_CPU_PSTATE "intel_pstate"
#define LINUX_CPU_PSTATE_STATUS "status"
#define LINUX_CPU_PSTATE_NOTURBO "no_turbo"
#define LINUX_CPU_PSTATE_MAX_PERF "max_perf_pct"
#define LINUX_CPU_PSTATE_MIN_PERF "min_perf_pct"

#define LINUX_CORETEMP "/sys/class/hwmon/hwmon%1"
#define LINUX_CORETEMP_CRIT "temp%1_crit"
#define LINUX_CORETEMP_INPUT "temp%1_input"
#define LINUX_CORETEMP_LABEL "temp%1_label"
#define LINUX_CORETEMP_MAX "temp%1_max"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
#define QT_SKIP_EMPTY Qt::SkipEmptyParts
#else
#define QT_SKIP_EMPTY QString::SkipEmptyParts
#endif

using namespace PowerKit;

int Cpu::getTotal()
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

const QString Cpu::getGovernor(int cpu)
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

const QStringList Cpu::getGovernors()
{
    QStringList result;
    for (int i=0;i<getTotal();++i) {
        QString value = getGovernor(i);
        if (!value.isEmpty()) { result << value; }
    }
    return result;
}

const QStringList Cpu::getAvailableGovernors()
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
        result = value.split(" ", QT_SKIP_EMPTY);
        gov.close();
    }
    return result;
}

bool Cpu::governorExists(const QString &gov)
{
    if (gov.isEmpty()) { return false; }
    return getAvailableGovernors().contains(gov);
}

bool Cpu::setGovernor(const QString &gov, int cpu)
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

bool Cpu::setGovernor(const QString &gov)
{
    if (!governorExists(gov)) { return false; }
    bool failed = false;
    for (int i=0;i<getTotal();++i) {
        if (!setGovernor(gov, i)) { failed = true; }
    }
    if (failed) { return false; }
    return true;
}

const QString Cpu::getFrequency(int cpu)
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

const QStringList Cpu::getFrequencies()
{
    QStringList result;
    for (int i=0;i<getTotal();++i) {
        QString value = getFrequency(i);
        if (!value.isEmpty()) { result << value; }
    }
    return result;
}

const QStringList Cpu::getAvailableFrequency()
{
    QStringList result;
    if (hasPState()) { return result; }
    QFile gov(QString("%1/cpu%2/%3/%4")
              .arg(LINUX_CPU_SYS)
              .arg(0)
              .arg(LINUX_CPU_DIR)
              .arg(LINUX_CPU_FREQUENCIES));
    if (!gov.exists()) { return result; }
    if (gov.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString value = gov.readAll().trimmed();
        result = value.split(" ", QT_SKIP_EMPTY);
        gov.close();
    }
    return result;
}

int Cpu::getMinFrequency()
{
    return getScalingFrequency(0, 0);
}

int Cpu::getMaxFrequency()
{
    return getScalingFrequency(0, 1);
}

int Cpu::getScalingFrequency(int cpu, int scale)
{
    int result = 0;
    QString type;
    switch (scale) {
    case 0:
        type = LINUX_CPU_FREQUENCY_MIN;
        break;
    case 1:
        type = LINUX_CPU_FREQUENCY_MAX;
        break;
    }
    QFile freq(QString("%1/cpu%2/%3/%4").arg(LINUX_CPU_SYS,
                                             QString::number(cpu),
                                             LINUX_CPU_DIR,
                                             type));
    if (freq.open(QIODevice::ReadOnly|QIODevice::Text)) {
        result = freq.readAll().trimmed().toInt();
        freq.close();
    }
    return result;
}

bool Cpu::frequencyExists(const QString &freq)
{
    if (hasPState()) { return false; }
    if (freq.isEmpty()) { return false; }
    return getAvailableFrequency().contains(freq);
}

bool Cpu::setFrequency(const QString &freq, int cpu)
{
    if (hasPState()) { return false; }
    if (!frequencyExists(freq)) { return false; }
    if (getGovernor(cpu) != "userspace") {
        if (!setGovernor("userspace", cpu)) { return false; }
    }
    QString val = freq;
    int freqMin = getMinFrequency();
    int freqMax = getMaxFrequency();
    if (freq.toInt() > freqMax) { val = QString::number(freqMax); }
    else if (freq.toInt() < freqMin) { val = QString::number(freqMin); }
    QFile file(QString("%1/cpu%2/%3/%4").arg(LINUX_CPU_SYS,
                                             QString::number(cpu),
                                             LINUX_CPU_DIR,
                                             LINUX_CPU_SET_SPEED));
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        QTextStream out(&file);
        out << val;
        file.close();
        if (val == getFrequency(cpu)) { return true; }
    }
    return false;
}

bool Cpu::setFrequency(const QString &freq)
{
    if (hasPState()) { return false; }
    if (!frequencyExists(freq)) { return false; }
    bool failed = false;
    for (int i=0;i<getTotal();++i) {
        if (!setFrequency(freq, i)) { failed = true; }
    }
    if (failed) { return false; }
    return true;
}

bool Cpu::hasPState()
{
    return QFile::exists(QString("%1/%2").arg(LINUX_CPU_SYS, LINUX_CPU_PSTATE));
}

bool Cpu::hasPStateTurbo()
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

bool Cpu::setPStateTurbo(bool turbo)
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

int Cpu::getPStateMax()
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

int Cpu::getPStateMin()
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

bool Cpu::setPStateMax(int maxState)
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

bool Cpu::setPStateMin(int minState)
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

bool Cpu::setPState(int min, int max)
{
    return (setPStateMin(min) && setPStateMax(max));
}

bool Cpu::hasCoreTemp()
{
    return QFile::exists(QString(LINUX_CORETEMP).arg(0));
}

QPair<double, double> Cpu::getCoreTemp()
{
    QPair<double, double> temp = {0.0, 0.0};
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
            QFile fileInput(QString("%1/%2").arg(QString(LINUX_CORETEMP).arg(hwmon),
                                                 QString(LINUX_CORETEMP_INPUT).arg(temps)));
            if (fileInput.open(QIODevice::ReadOnly|QIODevice::Text)) {
                double ctemp = fileInput.readAll().trimmed().toDouble();
                file.close();
                if (ctemp > temp.first) {
                    temp.first = ctemp;
                }
            }
            QFile fileMax(QString("%1/%2").arg(QString(LINUX_CORETEMP).arg(hwmon),
                                               QString(LINUX_CORETEMP_MAX).arg(temps)));
            if (fileMax.open(QIODevice::ReadOnly|QIODevice::Text)) {
                double mtemp = fileMax.readAll().trimmed().toDouble();
                file.close();
                if (mtemp > temp.second) {
                    temp.second = mtemp;
                }
            }
            if (temp.first > 0.0 && temp.second > 0.0) { break; }
        }
    }
    return temp;
}

const QPair<int, QString> Cpu::getCpuFreqLabel()
{
    QStringList freqs = getFrequencies();
    int currentCpuFreq = 0;
    double currentFancyFreq = 0.;
    for (int i=0; i < freqs.size(); ++i) {
        auto freq = freqs.at(i).toLong();
        if (freq > currentCpuFreq) {
            currentCpuFreq = freq;
            currentFancyFreq = freqs.at(i).toDouble();
        }
    }

    QPair<int, QString> result;

    int freqMin = Cpu::getMinFrequency();
    int freqMax = Cpu::getMaxFrequency();
    int progress;
    if (freqMax == freqMin) {
        progress = 100;
    } else {
        progress = ((currentCpuFreq - freqMin) * 100) / (freqMax - freqMin);
    }

    result.first = progress;
    result.second = QString("%1\nGhz").arg(QString::number(currentFancyFreq / 1000000, 'f', 2));
    return result;
}

const QPair<int, QString> Cpu::getCpuTempLabel()
{
    QPair<int, QString> result;
    const auto temps = getCoreTemp();
    int progress = (temps.first * 100) / temps.second;

    result.first = progress;
    result.second = QString("%1°C").arg(QString::number(temps.first / 1000, 'f', 0));
    return result;
}
