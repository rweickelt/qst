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
    results = execQstRun(QStringList{ "-f", dataPath("project/project-inline-testcases.qml") });
    if (qstProcess().exitCode() != 0)
    {
        QFAIL(qstProcess().readAllStandardError());
    }
    VERIFY_PASS(results, "inline-1");
    VERIFY_PASS(results, "inline-2");
    VERIFY_PASS(results, "test1");
    VERIFY_PASS(results, "test2");
    QByteArray stdError = qstProcess().readAllStandardError();
    QVERIFY(stdError.contains("inline-1 run"));
    QVERIFY(stdError.contains("inline-2 run"));
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
    if (qstProcess().exitCode() != 0)
    {
        QFAIL(qstProcess().readAllStandardError());
    }
    VERIFY_PASS(results, "name");
    VERIFY_PASS(results, "testcase-0");

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
            << QString(".project-with-name-default-%1").arg(qHash(projectDir.absoluteFilePath("project-with-name.qml")), 0, 16);

    QTest::newRow("without name, no override")
            << projectDir.absoluteFilePath("project-without-name.qml")
            << false
            << QString(".project-default-%1").arg(qHash(projectDir.absoluteFilePath("project-without-name.qml")), 0, 16);
}

void AutoTest::workingDirectory()
{
    QFETCH(QString, projectFilepath);
    QFETCH(bool, overrideWorkingDir);
    QFETCH(QString, workingDirName);

    QVERIFY2(QFileInfo(projectFilepath).exists(), "projectFilePath does not exist.");

    QString workDirPath = QDir(qstProcess().workingDirectory()).absoluteFilePath(workingDirName);
    QDir workDir(workDirPath);

    // Make sure that any old left-overs are removed
    workDir.removeRecursively();
    QVERIFY(!workDir.exists());

    QStringList arguments = { "-f", projectFilepath };
    if (overrideWorkingDir == true)
    {
        arguments << "-d" << workDirPath;
    }
    QstTestResults results = execQstRun(arguments);
    QByteArray stdErr = qstProcess().readAllStandardError();
    QVERIFY2(qstProcess().exitCode() == 0, stdErr);
    if (!stdErr.isEmpty())
    {
        qDebug() << stdErr;
    }
    VERIFY_PASS(results, "testcase");
    QVERIFY(workDir.exists());
    QVERIFY(workDir.exists("testcase"));
}

void AutoTest::signalProbe()
{
    QstTestResults results = execQstRun(QStringList{ "-f", dataPath("signalprobe/project.qml")} );
    if (qstProcess().exitCode() != 2)
    {
        QFAIL(qstProcess().readAllStandardError());
    }
    VERIFY_PASS(results, "verify-triggered");
    VERIFY_FAIL(results, "nonexisting-signal", "SignalProbe.qml:33");
}

void AutoTest::durationConstraint()
{
    QstTestResults results = execQstRun(QStringList{ "-f", dataPath("durationconstraint/project.qml")} );
    if (qstProcess().exitCode() != 2)
    {
        QFAIL(qstProcess().readAllStandardError());
    }
    VERIFY_PASS(results, "duration-inside-range");
    VERIFY_FAIL(results, "duration-above-max", "DurationConstraint.qml:82");
    VERIFY_FAIL(results, "duration-below-min", "DurationConstraint.qml:82");
    VERIFY_FAIL(results, "evaluate-on-finished", "DurationConstraint.qml:82");
    QVERIFY(qstProcess().readAllStandardError().contains("checkpoint passed"));

    results = execQstRun(QStringList{ "-f", dataPath("durationconstraint/fail-on-non-existing-begin-signal.qml")} );
    if (qstProcess().exitCode() != 1)
    {
        QFAIL(qstProcess().readAllStandardError());
    }

    results = execQstRun(QStringList{ "-f", dataPath("durationconstraint/fail-on-non-existing-end-signal.qml")} );
    if (qstProcess().exitCode() != 1)
    {
        QFAIL(qstProcess().readAllStandardError());
    }
}

void AutoTest::valueRangeConstraint()
{
    QstTestResults results = execQstRun(QStringList{ "-f", dataPath("valuerangeconstraint/project.qml")} );
    if (qstProcess().exitCode() != 2)
    {
        QFAIL(qstProcess().readAllStandardError());
    }
    VERIFY_FAIL(results, "range", "ValueRangeConstraint.qml:24");
    VERIFY_FAIL(results, "evaluate-on-finished", "ValueRangeConstraint.qml:24");
    QVERIFY(qstProcess().readAllStandardError().contains("checkpoint passed"));
}

void AutoTest::profile()
{
    QstTestResults results;
    QByteArray stdErr;
    // Expected profile search order:
    // 1. explicitly given -P
    // 2. testfile dir
    // 3. config dir

    // Profile in given path -P
    results = execQstRun(QStringList{
                                            "-f", dataPath("profile/existing-profile-properties.qml"),
                                            "-p", "profile-1",
                                            "-P", dataPath("profile/explicit-directory-2")
                                        });
    if (qstProcess().exitCode() != qst::ExitNormal) { QFAIL(qstProcess().readAllStandardError()); }
    stdErr = qstProcess().readAllStandardError();
    QVERIFY(stdErr.contains("location:explicit-2"));
    QVERIFY(stdErr.contains("variant:1"));

    // In one of multiple -P directories
    results = execQstRun(QStringList{
                                            "-f", dataPath("profile/existing-profile-properties.qml"),
                                            "-p", "profile-1",
                                            "-P", dataPath("profile/explicit-directory-1"),
                                            "-P", dataPath("profile/explicit-directory-2")
                                        });
    if (qstProcess().exitCode() != qst::ExitNormal) { QFAIL(qstProcess().readAllStandardError()); }
    stdErr = qstProcess().readAllStandardError();
    QVERIFY(stdErr.contains("location:explicit-2"));
    QVERIFY(stdErr.contains("variant:1"));

    // Profile in testfile dir (implicit)
    results = execQstRun(QStringList{
                                            "-f", dataPath("profile/existing-profile-properties.qml"),
                                            "-p", "profile-1"
                                        });
    if (qstProcess().exitCode() != qst::ExitNormal) { QFAIL(qstProcess().readAllStandardError()); }
    stdErr = qstProcess().readAllStandardError();
    QVERIFY(stdErr.contains("location:project"));
    QVERIFY(stdErr.contains("variant:1"));

    // Profile in config dir (implicit)
    // Missing

    // Specifying a profile that doesn't exist should error.
    results = execQstRun(QStringList{
                                            "-f", dataPath("profile/existing-profile-properties.qml"),
                                            "-p", "doesnt-exist"
                                        });
    QVERIFY2(qstProcess().exitCode() == qst::ExitApplicationError, results.stdOut());

    // Broken JSON file should error.
    results = execQstRun(QStringList{
                                            "-f", dataPath("profile/existing-profile-properties.qml"),
                                            "-p", "broken-profile.json"
                                        });
    QVERIFY2(qstProcess().exitCode() == qst::ExitApplicationError, results.stdOut());

    // Non-existing profile properties in a project item should fail
    results = execQstRun(QStringList{
                            "-f", dataPath("profile/non-existing-profile-properties-project.qml"),
                            "-p", "profile-1"
                         });
    QCOMPARE(qstProcess().exitCode(), static_cast<int>(qst::ExitApplicationError));

    // Non-existing profile properties in a run() method should fail.
    results = execQstRun(QStringList{
                            "-f", dataPath("profile/non-existing-profile-properties-run.qml"),
                            "-p", "profile-1"
                         });
    QCOMPARE(qstProcess().exitCode(), static_cast<int>(qst::ExitTestCaseFailed));
    VERIFY_FAIL(results, "non-existing-profile-properties-run", "non-existing-profile-properties-run.qml:9");
}

void AutoTest::matrix()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("matrix/project-ok.qml"));
    VERIFY_PASS(results(), "testcase 1scca4h [ cat white ]");
    VERIFY_PASS(results(), "testcase 1gmluzj [ dog brown ]");
    VERIFY_PASS(results(), "testcase 0wgajg3 [ cat brown ]");
    VERIFY_PASS(results(), "testcase 0cyywtp [ dog white ]");

    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("matrix/project-wildcard.qml"));
    VERIFY_PASS(results(), "testcase 1scca4h [ cat white ]");
    VERIFY_PASS(results(), "testcase 1gmluzj [ dog brown ]");
    VERIFY_PASS(results(), "testcase 0wgajg3 [ cat brown ]");
    VERIFY_PASS(results(), "testcase 0cyywtp [ dog white ]");
    VERIFY_PASS(results(), "othertestcase 1scca4h [ cat white ]");
    VERIFY_PASS(results(), "othertestcase 1gmluzj [ dog brown ]");
    VERIFY_PASS(results(), "othertestcase 0wgajg3 [ cat brown ]");
    VERIFY_PASS(results(), "othertestcase 0cyywtp [ dog white ]");
    VERIFY_PASS(results(), "toastcase");

    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("matrix/project-with-external-matrix.qml"));
    VERIFY_PASS(results(), "testcase 1scca4h [ cat white ]");
    VERIFY_PASS(results(), "testcase 1gmluzj [ dog brown ]");
    VERIFY_PASS(results(), "testcase 0wgajg3 [ cat brown ]");
    VERIFY_PASS(results(), "testcase 0cyywtp [ dog white ]");

    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("matrix/project-overlapping-matrices.qml"));
    VERIFY_PASS(results(), "testcase 1scca4h [ cat white ]");
    VERIFY_PASS(results(), "testcase 1gmluzj [ dog brown ]");
    VERIFY_PASS(results(), "testcase 0wgajg3 [ cat brown ]");
    VERIFY_PASS(results(), "testcase 0cyywtp [ dog white ]");
    VERIFY_PASS(results(), "testcase 1v8a6z7 [ hog brown ]");
    VERIFY_PASS(results(), "testcase 11g8wzx [ bat brown ]");
    VERIFY_PASS(results(), "testcase 0r0l9i9 [ hog white ]");
    VERIFY_PASS(results(), "testcase 062v5j3 [ bat white ]");

    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("matrix/matrix-inside-testcase-ok.qml"));
    VERIFY_PASS(results(), "testcase 1scca4h [ cat white ]");
    VERIFY_PASS(results(), "testcase 1gmluzj [ dog brown ]");
    VERIFY_PASS(results(), "testcase 0wgajg3 [ cat brown ]");
    VERIFY_PASS(results(), "testcase 0cyywtp [ dog white ]");

    RUN_AND_EXPECT(qst::ExitApplicationError, "-f", dataPath("matrix/project-overlapping-dimensions.qml"));
    QVERIFY(stdError().contains("project-overlapping-dimensions.qml:6"));

    RUN_AND_EXPECT(qst::ExitApplicationError, "-f", dataPath("matrix/project-no-matching-testcase.qml"));
    QVERIFY(stdError().contains("project-no-matching-testcase.qml:6"));

    RUN_AND_EXPECT(qst::ExitApplicationError, "-f", dataPath("matrix/project-malformed-dimensions.qml"));
    QVERIFY(stdError().contains("project-malformed-dimensions.qml:11"));

    RUN_AND_EXPECT(qst::ExitApplicationError, "-f", dataPath("matrix/project-testcase-incompatible-properties.qml"));
    QVERIFY(stdError().contains("testcase.qml:3"));

    RUN_AND_EXPECT(qst::ExitApplicationError, "-f", dataPath("matrix/matrix-inside-testcase-protected-testcases.qml"));
    QVERIFY(stdError().contains("matrix-inside-testcase-protected-testcases.qml:9"));
}

void AutoTest::codeSnippets()
{
    QstTestResults results = execQstRun(QStringList{ "-f", dataPath("code-snippets/usage.qml")} );
    if (qstProcess().exitCode() != 2)
    {
        QFAIL(qstProcess().readAllStandardError());
    }
    VERIFY_PASS(results, "simple-passing-test");
    VERIFY_PASS(results, "makefile-test-simple");
    VERIFY_PASS(results, "test-app-build");
    VERIFY_PASS(results, "test-lib-build");
    VERIFY_FAIL(results, "simple-failing-test", "simple-failing-test.qml:8");
}

QTEST_GUILESS_MAIN(AutoTest)
