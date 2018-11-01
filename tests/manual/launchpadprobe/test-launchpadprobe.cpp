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
#include <qst.h>
#include <qtest.h>
#include <qsttestresults.h>

#include <qsttest.h>

#include <QtCore/QtGlobal>
#include <QtCore/QObject>
#include <QtTest/QtTest>

class test_launchpadprobe : public QstTest
{
    Q_OBJECT

public:
    QString dataPath(const QString &fileName) const;

private slots:
    void init();
    void flashFirmware();
    void pinProbereadWrite();
    void benchmarkPinProbeResponseTime();
    void pinProbeWatchdog();

private:
    QString profile() const;
};

QString test_launchpadprobe::profile() const
{
#ifdef Q_OS_WIN
    return "cc1310_launchxl_windows";
#elif defined Q_OS_LINUX
    return "cc1310_launchxl_linux";
#endif
}

void test_launchpadprobe::init()
{
    setTimeoutMs(25000);
}

QString test_launchpadprobe::dataPath(const QString &fileName) const
{
    return QDir(QString(SOURCE_DIR)).absoluteFilePath(fileName);
}

void test_launchpadprobe::flashFirmware()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("flash-firmware.qml"), "-p", profile());
    VERIFY_PASS(results(), "flash-firmware");
}

void test_launchpadprobe::pinProbereadWrite()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("pinprobe-read-write.qml"), "-p", profile());
    VERIFY_PASS(results(), "pinprobe-read-write");

}

void test_launchpadprobe::benchmarkPinProbeResponseTime()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("benchmark-response-time.qml"), "-p", profile());
    VERIFY_PASS(results(), "benchmark-response-time");

}

void test_launchpadprobe::pinProbeWatchdog()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("watchdog.qml"), "-p", profile());
    VERIFY_PASS(results(), "watchdog");
}

QTEST_GUILESS_MAIN(test_launchpadprobe)

#include "test-launchpadprobe.moc"
