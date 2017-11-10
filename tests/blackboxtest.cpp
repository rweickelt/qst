/****************************************************************************
 **
 ** Copyright (C) 2017 The Qst project.
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

#include "blackboxtest.h"
#include "qsttestresults.h"


#include <QtCore/QDir>
#include <QtCore/QRegularExpression>
#include <qtest.h>

#if ! defined(PROJECTPATH)
#error PROJECTPATH is not defined.
#endif


#define VERIFY_PASS(name) \
    if (!qstResults.contains(name)) { \
             QFAIL(qPrintable(QString("%1: Not found in output") \
                    .arg(name))); } \
    if (qstResults.output(name).result != "PASS") { \
             QFAIL(qPrintable(QString("%1: Expected PASS but failed, '%2'") \
                    .arg(name) \
                    .arg(qstResults.output(name).message))); \
    }


#define VERIFY_FAIL(name, expectedLocation) \
    if (!qstResults.contains(name)) { \
             QFAIL(qPrintable(QString("%1: Not found in output") \
                    .arg(name))); } \
    if (qstResults.output(name).result != "FAIL") { \
        QFAIL(qPrintable(QString("%1: Expected FAIL but passed") \
               .arg(name))); \
    } \
    if (!qstResults.output(name).location.endsWith(expectedLocation)) { \
             QFAIL(qPrintable(QString("%1: Expected location '%2', Actual location '%3'") \
                    .arg(name) \
                    .arg(expectedLocation) \
                    .arg(qstResults.output(name).location))); \
    }

namespace {
    const QDir defaultImportDir = QDir(PROJECTPATH "/share/qst/imports/");

    QProcess qstProcess;
    QstTestResults qstResults;
}


QString dataPath(const QString& directory)
{
    return QDir(PROJECTPATH "/tests/testdata/").absoluteFilePath(directory);
}

void execQstRun(const QStringList& arguments)
{
    QStringList cmdLine = { "run", "--import", defaultImportDir.absolutePath() };
    cmdLine += arguments;
    QDir qstDir(QCoreApplication::applicationDirPath());
    qstProcess.start(qstDir.absoluteFilePath("qst"), cmdLine);
    QVERIFY(qstProcess.waitForFinished(500));

    qstResults = QstTestResults::fromQstOutput(qstProcess.readAllStandardOutput());
}

BlackboxTest::BlackboxTest(QObject *parent) : QObject(parent) {}

void BlackboxTest::project()
{
    execQstRun(QStringList{ "-f", dataPath("project/project.qml") });
    if (qstProcess.exitCode() != 0)
    {
        QFAIL(qstProcess.readAllStandardError());
    }
    VERIFY_PASS("test1");
    VERIFY_PASS("test2");
}

void BlackboxTest::testCase()
{
    execQstRun(QStringList{ "-f", dataPath("testcase/project.qml") });
    if (qstProcess.exitCode() != 2)
    {
        QFAIL(qstProcess.readAllStandardError());
    }
    VERIFY_PASS("empty-run-function");
    VERIFY_PASS("verify-pass");
    VERIFY_FAIL("verify-fail-run", "verify-fail-run.qml:8");
    VERIFY_FAIL("verify-fail-onStarted", "verify-fail-onStarted.qml:8");
    VERIFY_FAIL("verify-fail-onFinished", "verify-fail-onFinished.qml:10");
    VERIFY_PASS("signals-and-attached-signals");
    VERIFY_FAIL("verify-fail-attached-signal", "verify-fail-attached-signal.qml:11");
    VERIFY_FAIL("verify-fail-child-component", "ChildComponent.qml:8");
    VERIFY_FAIL("verify-fail-in-component-signal-handler", "verify-fail-in-component-signal-handler.qml:13");
    VERIFY_FAIL("compare-bool", "compare-bool.qml:9");
    VERIFY_FAIL("compare-integer", "compare-integer.qml:9");
    VERIFY_FAIL("compare-string", "compare-string.qml:9");
    VERIFY_FAIL("compare-stringlist", "compare-stringlist.qml:25");
    VERIFY_PASS("wait");
}

void BlackboxTest::testCaseName()
{
    execQstRun(QStringList{ "-f", dataPath("testcase-name/project-missing-name.qml") });
    if (qstProcess.exitCode() != 1)
    {
        QFAIL(qstProcess.readAllStandardError());
    }
    execQstRun(QStringList{ "-f", dataPath("testcase-name/project-duplicate-name.qml") });
    if (qstProcess.exitCode() != 1)
    {
        QFAIL(qstProcess.readAllStandardError());
    }
}


void BlackboxTest::errorHandling()
{
    execQstRun(QStringList{ "-f", dataPath("error-handling/project.qml")} );
    if (qstProcess.exitCode() != 2)
    {
        QFAIL(qstProcess.readAllStandardError());
    }
    VERIFY_FAIL("error-in-run-function", "error-in-run-function.qml:8");
    VERIFY_FAIL("error-in-signal-handler", "error-in-signal-handler.qml:10");
}

void BlackboxTest::processProbe()
{
    execQstRun(QStringList{ "-f", dataPath("processprobe/project.qml")} );
    if (qstProcess.exitCode() != 0)
    {
        QFAIL(qstProcess.readAllStandardError());
    }
    VERIFY_PASS("arguments");
    VERIFY_PASS("onErrorOccured");
}

void BlackboxTest::workingDirectory_data()
{
    QTest::addColumn<QString>("projectFilepath");
    QTest::addColumn<bool>("overrideWorkingDir");
    QTest::addColumn<QString>("workingDirName");

    QDir projectDir = dataPath("working-directory");

    QTest::newRow("with name, override")
            << projectDir.absoluteFilePath("project-with-name.qml")
            << true
            << "qst-project";

    QTest::newRow("without name, override")
            << projectDir.absoluteFilePath("project-without-name.qml")
            << true
            << "project-without-name";

    QTest::newRow("with name, no override")
            << projectDir.absoluteFilePath("project-with-name.qml")
            << false
            << QString("project-with-name-%1").arg(qHash(projectDir.absoluteFilePath("project-with-name.qml")), 0, 16);

    QTest::newRow("without name, no override")
            << projectDir.absoluteFilePath("project-without-name.qml")
            << false
            << QString("qst-project-%1").arg(qHash(projectDir.absoluteFilePath("project-without-name.qml")), 0, 16);
}

void BlackboxTest::workingDirectory()
{
    QFETCH(QString, projectFilepath);
    QFETCH(bool, overrideWorkingDir);
    QFETCH(QString, workingDirName);

    QVERIFY2(QFileInfo(projectFilepath).exists(), "projectFilePath does not exist.");

    QString tempDirPath = QDir::temp().absoluteFilePath(workingDirName);
    QDir tempDir(tempDirPath);

    // Make sure that any old left-overs are removed
    tempDir.removeRecursively();
    QVERIFY(!tempDir.exists());

    QStringList arguments = { "-f", projectFilepath };
    if (overrideWorkingDir == true)
    {
        arguments << "-d" << QDir::temp().absoluteFilePath(workingDirName);
    }
    execQstRun(arguments);
    QVERIFY2(qstProcess.exitCode() == 0, qstProcess.readAllStandardError());
    VERIFY_PASS("testcase");
    QVERIFY(tempDir.exists());
    QVERIFY(tempDir.exists("testcase"));
}

void BlackboxTest::pinProbe()
{
    execQstRun(QStringList{ "-f", dataPath("pinprobe/readwrite.qml") });
    if (qstProcess.exitCode() != 2)
    {
        QFAIL(qstProcess.readAllStandardError());
    }
    VERIFY_PASS("readwrite");
}


QTEST_GUILESS_MAIN(BlackboxTest)
