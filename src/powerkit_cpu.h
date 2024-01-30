/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWERKIT_CPU_H
#define POWERKIT_CPU_H

#include <QStringList>

namespace PowerKit
{
    class Cpu
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
        static int getMinFrequency();
        static int getMaxFrequency();
        static int getScalingFrequency(int cpu, int scale);
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
        static bool setPState(int min, int max);

        static bool hasCoreTemp();
        static int getCoreTemp();
    };
}

#endif // POWERKIT_CPU_H
