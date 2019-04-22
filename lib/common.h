/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef COMMON_H
#define COMMON_H

#include <QVariant>
#include <QString>
#include <QDBusInterface>

#ifdef Q_OS_LINUX
#define LINUX_CPU_SYS "/sys/devices/system/cpu"
#define LINUX_CPU_DIR "cpufreq"
#define LINUX_CPU_FREQUENCIES "scaling_available_frequencies"
#define LINUX_CPU_FREQUENCY "scaling_cur_freq"
#define LINUX_CPU_FREQUENCY_MAX "scaling_max_freq"
#define LINUX_CPU_FREQUENCY_MIN "scaling_min_freq"
#define LINUX_CPU_GOVERNORS "scaling_available_governors"
#define LINUX_CPU_GOVERNOR "scaling_governor"
#define LINUX_CPU_SET_SPEED "scaling_setspeed"
#endif

class Common
{
public:
    static void savePowerSettings(QString type, QVariant value);
    static QVariant loadPowerSettings(QString type);
    static bool validPowerSettings(QString type);
    static void saveDefaultSettings();
    //static void setIconTheme();
    static QString confFile();
    static QString confDir();
    static bool kernelCanResume(bool ignore = false /* if ignore then always return true */);
    static QString backlightDevice();
    static bool canAdjustBacklight(QString device);
    static int backlightMax(QString device);
    static int backlightValue(QString device);
    static bool adjustBacklight(QString device, int value);
    static void checkSettings();

    static int getCpuTotal();

    static const QString getCpuGovernor(int cpu);
    static const QStringList getCpuGovernors();
    static const QStringList getAvailableGovernors();
    static bool cpuGovernorExists(const QString &gov);
    static bool setCpuGovernor(const QString &gov, int cpu);
    static bool setCpuGovernor(const QString &gov);

    static const QString getCpuFrequency(int cpu);
    static const QStringList getCpuFrequencies();
    static const QStringList getCpuAvailableFrequency();
    static bool cpuFrequencyExists(const QString &freq);
    static bool setCpuFrequency(const QString &freq, int cpu);
    static bool setCpuFrequency(const QString &freq);

    static double getBatteryLeft(QDBusInterface *iface);
    static bool hasBattery(QDBusInterface *iface);
    static bool onBattery(QDBusInterface *iface);
    static qlonglong timeToEmpty(QDBusInterface *iface);
    static bool canHibernate(QDBusInterface *iface);
    static bool canSuspend(QDBusInterface *iface);
    static bool lidIsPresent(QDBusInterface *iface);
};

#endif // COMMON_H
