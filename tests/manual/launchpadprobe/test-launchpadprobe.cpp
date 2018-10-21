/****************************************************************************
 **
 ** Copyright (C) 2017, 2018 The Qst project.
 **
 ** Contact: https://github.com/rweickelt/qst
 **
 ** $BEGIN_LICENSE$
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.

 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.

 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **
 ** $END_LICENSE$
****************************************************************************/
#include "test-launchpadprobe.h"
#include <qtest.h>
#include <qsttestresults.h>

QString LaunchpadProbeTest::dataPath(const QString &fileName) const
{
    return QDir(QString(SOURCE_DIR)).absoluteFilePath(fileName);
}

void LaunchpadProbeTest::flashFirmware()
{
    QstTestResults results = execQstRun(QStringList{ "-f", dataPath("flash-firmware.qml"), "-p", "cc1310_launchxl" }, 25000);
    if (qstProcess().exitCode() != 0)
    {
        QFAIL(qstProcess().readAllStandardError());
    }
    VERIFY_PASS(results, "flash-firmware");
}

void LaunchpadProbeTest::pinProbereadWrite()
{
    QstTestResults results = execQstRun(QStringList{ "-f", dataPath("pinprobe-read-write.qml"), "-p", "cc1310_launchxl" }, 25000);
    if (qstProcess().exitCode() != 0)
    {
        QFAIL(qstProcess().readAllStandardError());
    }
    VERIFY_PASS(results, "pinprobe-read-write");
}

void LaunchpadProbeTest::benchmarkPinProbeResponseTime()
{
    QstTestResults results = execQstRun(QStringList{ "-f", dataPath("benchmark-response-time.qml"), "-p", "cc1310_launchxl" }, 25000);
    if (qstProcess().exitCode() != 0)
    {
        QFAIL(qstProcess().readAllStandardError());
    }
    VERIFY_PASS(results, "benchmark-response-time");
}

void LaunchpadProbeTest::pinProbeWatchdog()
{
    {
        QstTestResults results = execQstRun(QStringList{ "-f", dataPath("watchdog.qml"), "-p", "cc1310_launchxl" }, 25000);
        if (qstProcess().exitCode() != 0)
        {
            QFAIL(qstProcess().readAllStandardError());
        }
        VERIFY_PASS(results, "watchdog");
    }
}

QTEST_GUILESS_MAIN(LaunchpadProbeTest)
