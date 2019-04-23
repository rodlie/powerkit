/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
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
};

#endif // POWER_CPU_H
