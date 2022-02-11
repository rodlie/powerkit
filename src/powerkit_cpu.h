/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018-2022 Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWER_CPU_H
#define POWER_CPU_H

#include <QStringList>
#include <QDBusInterface>

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

#define LINUX_CORETEMP "/sys/class/hwmon/hwmon0"
#define LINUX_CORETEMP_CRIT "temp%1_crit"
#define LINUX_CORETEMP_INPUT "temp%1_input"
#define LINUX_CORETEMP_LABEL "temp%1_label"
#define LINUX_CORETEMP_MAX "temp%1_max"

class PowerCpu
{
public:
    static int getTotal();

    static const QString getGovernor(int cpu);
    static const QStringList getGovernors();
    static const QStringList getAvailableGovernors();
    static bool governorExists(const QString &gov);
    static bool setGovernor(const QString &gov, int cpu);
    static bool setGovernor(const QString &gov);

    static const QString getFrequency(int cpu);
    static const QStringList getFrequencies();
    static const QStringList getAvailableFrequency();
    static bool frequencyExists(const QString &freq);
    static bool setFrequency(const QString &freq, int cpu);
    static bool setFrequency(const QString &freq);

    static bool hasPState();
    static bool hasPStateTurbo();
    static bool setPStateTurbo(bool turbo);
    static int getPStateMax();
    static int getPStateMin();
    static bool setPStateMax(int maxState);
    static bool setPStateMin(int minState);

    static bool hasCoreTemp();
    static int getCoreTemp();
};

#endif // POWER_CPU_H
