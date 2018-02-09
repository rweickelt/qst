/****************************************************************************
 **
 ** Copyright (C) 2017-2018 The Qst project.
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

#include "autotest.h"
#include <qsttestresults.h>


#include <QtCore/QDir>
#include <QtCore/QRegularExpression>
#include <qtest.h>
#include <qst.h>

void AutoTest::project()
{
    QstTestResults results = execQstRun(QStringList{ "-f", dataPath("project/project-no-errors.qml") });
    if (qstProcess().exitCode() != 0)
    {
        QFAIL(qstProcess().readAllStandardError());
    }
    VERIFY_PASS(results, "test1");
    VERIFY_PASS(results, "test2");
    results = execQstRun(QStringList{ "-f", dataPath("project/project-verify-fail.qml") });
    if (qstProcess().exitCode() != qst::ExitApplicationError)
    {
        QFAIL(qstProcess().readAllStandardOutput());
    }
    results = execQstRun(QStringList{ "-f", dataPath("project/project-compare-fail.qml") });
    if (qstProcess().exitCode() != qst::ExitApplicationError)
    {
        QFAIL(qstProcess().readAllStandardOutput());
    }
}

void AutoTest::testCase()
{
    QstTestResults results = execQstRun(QStringList{ "-f", dataPath("testcase/project.qml") });
    if (qstProcess().exitCode() != 2)
    {
        QFAIL(qstProcess().readAllStandardError());
    }
    VERIFY_PASS(results, "empty-run-function");
    VERIFY_PASS(results, "verify-pass");
    VERIFY_FAIL(results, "verify-fail-run", "verify-fail-run.qml:8");
    VERIFY_FAIL(results, "verify-fail-onStarted", "verify-fail-onStarted.qml:8");
    VERIFY_FAIL(results, "verify-fail-onFinished", "verify-fail-onFinished.qml:10");
    VERIFY_PASS(results, "signals-and-attached-signals");
    VERIFY_FAIL(results, "verify-fail-attached-signal", "verify-fail-attached-signal.qml:11");
    VERIFY_FAIL(results, "verify-fail-child-component", "ChildComponent.qml:8");
    VERIFY_FAIL(results, "verify-fail-in-component-signal-handler", "verify-fail-in-component-signal-handler.qml:13");
    VERIFY_FAIL(results, "compare-bool", "compare-bool.qml:9");
    VERIFY_FAIL(results, "compare-integer", "compare-integer.qml:9");
    VERIFY_FAIL(results, "compare-string", "compare-string.qml:9");
    VERIFY_FAIL(results, "compare-stringlist", "compare-stringlist.qml:25");
    VERIFY_PASS(results, "wait");
}

void AutoTest::testCaseName()
{
    QstTestResults results = execQstRun(QStringList{ "-f", dataPath("testcase-name/project-missing-name.qml") });
    if (qstProcess().exitCode() != 1)
    {
        QFAIL(qstProcess().readAllStandardError());
    }
    results = execQstRun(QStringList{ "-f", dataPath("testcase-name/project-duplicate-name.qml") });
    if (qstProcess().exitCode() != 1)
    {
        QFAIL(qstProcess().readAllStandardError());
    }
}


void AutoTest::errorHandling()
{
    QstTestResults results = execQstRun(QStringList{ "-f", dataPath("error-handling/project.qml")} );
    if (qstProcess().exitCode() != 2)
    {
        QFAIL(qstProcess().readAllStandardError());
    }
    VERIFY_FAIL(results, "error-in-run-function", "error-in-run-function.qml:8");
    VERIFY_FAIL(results, "error-in-signal-handler", "error-in-signal-handler.qml:10");
}

void AutoTest::processProbe()
{
    QstTestResults results = execQstRun(QStringList{ "-f", dataPath("processprobe/project.qml")} );
    if (qstProcess().exitCode() != 0)
    {
        QFAIL(qstProcess().readAllStandardError());
    }
    VERIFY_PASS(results, "arguments");
    VERIFY_PASS(results, "onErrorOccured");
}

void AutoTest::workingDirectory_data()
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

void AutoTest::workingDirectory()
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
    QstTestResults results = execQstRun(arguments);
    QVERIFY2(qstProcess().exitCode() == 0, qstProcess().readAllStandardError());
    VERIFY_PASS(results, "testcase");
    QVERIFY(tempDir.exists());
    QVERIFY(tempDir.exists("testcase"));
}

void AutoTest::signalProbe()
{
    QstTestResults results = execQstRun(QStringList{ "-f", dataPath("signalprobe/project.qml")} );
    if (qstProcess().exitCode() != 2)
    {
        QFAIL(qstProcess().readAllStandardError());
    }
    VERIFY_PASS(results, "verify-triggered");
    VERIFY_FAIL(results, "nonexisting-signal", "SignalProbe.qml:26");
}


QTEST_GUILESS_MAIN(AutoTest)
