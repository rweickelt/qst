/****************************************************************************
 **
 ** Copyright (C) 2018 The Qst project.
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

#include <qsttest.h>
#include <qst.h>

class test_codesnippets : public QstTest
{
    Q_OBJECT
public:
    QString dataPath(const QString &fileName) const;

private slots:
    void depends();
    void dimension();
    void exports();
    void matrix();
    void usage();
};

QString test_codesnippets::dataPath(const QString &fileName) const
{
    return QDir(QString(SOURCE_DIR)).absoluteFilePath(fileName);
}

void test_codesnippets::depends()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("reference/simple-depends.qml"));
    QCOMPARE(results().passCount(), 2);
    VERIFY_EXECUTION_ORDER({"A", "B"});

    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("reference/depends-project-with-matrix.qml"));
    QCOMPARE(results().passCount(), 9);
    VERIFY_EXECUTION_ORDER({"A 0000000 [ board-2 release ]", "implicit 0000004 [ board-2 release ]"});
    VERIFY_EXECUTION_ORDER({"A 0000001 [ board-1 release ]", "implicit 0000005 [ board-1 release ]"});
    VERIFY_EXECUTION_ORDER({"A 0000002 [ board-2 debug ]", "implicit 0000006 [ board-2 debug ]"});
    VERIFY_EXECUTION_ORDER({"A 0000003 [ board-1 debug ]", "implicit 0000007 [ board-1 debug ]"});

    VERIFY_EXECUTION_ORDER({"A 0000002 [ board-2 debug ]", "explicit"});
    VERIFY_EXECUTION_ORDER({"A 0000003 [ board-1 debug ]", "explicit"});


    // Todo: Rather compare against terminal code snippet in the .rst file
}

void test_codesnippets::dimension()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("reference/dimension-with-references.qml"));
    VERIFY_PASS(results(), "testcase 0000000 [ dog white ]");
    VERIFY_PASS(results(), "testcase 0000001 [ cat white ]");
    VERIFY_PASS(results(), "testcase 0000002 [ dog brown ]");
    VERIFY_PASS(results(), "testcase 0000003 [ cat brown ]");
}


void test_codesnippets::exports()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("reference/simple-exports.qml"));
    QCOMPARE(results().passCount(), 2);
    VERIFY_EXECUTION_ORDER({"A", "B"});

    // Todo: Rather compare against terminal code snippet in the .rst file
}


void test_codesnippets::matrix()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("reference/matrix-project.qml"));
    VERIFY_PASS(results(), "tagged-test 0000000 [ dog bites ]");
    VERIFY_PASS(results(), "tagged-test 0000001 [ cat bites ]");
    VERIFY_PASS(results(), "tagged-test 0000002 [ dog moans ]");
    VERIFY_PASS(results(), "tagged-test 0000003 [ cat moans ]");
}



void test_codesnippets::usage()
{
    RUN_AND_EXPECT(qst::ExitTestCaseFailed, "-f", dataPath("usage/usage.qml"));
    VERIFY_PASS(results(), "simple-passing-test");
    VERIFY_PASS(results(), "makefile-test-simple");
    VERIFY_PASS(results(), "test-app-build");
    VERIFY_PASS(results(), "test-lib-build");
    VERIFY_FAIL(results(), "simple-failing-test", "simple-failing-test.qml:8");
}

QTEST_MAIN(test_codesnippets)

#include "test-codesnippets.moc"

