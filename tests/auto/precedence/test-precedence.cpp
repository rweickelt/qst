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

class test_precedence : public QstTest
{
    Q_OBJECT
public:
    QString dataPath(const QString &fileName) const;

private slots:
    void inlineNontaggedDependsNonTagged();
    void inlineNontaggedDependsSingleTagged();
    void inlineNontaggedDependsMultipleTagged();
    void inlineNontaggedDependsNontaggedWrongName();
    void inlineTaggedDependsNontagged();
    void inlineTaggedDependsSingleTagged();
    void inlineTaggedDependsMultipleTagged();
    void inlineNontaggedExportsIllegalName();
    void inlineCycle();
};

QString test_precedence::dataPath(const QString &fileName) const
{
    return QDir(QString(SOURCE_DIR)).absoluteFilePath(fileName);
}

void test_precedence::inlineNontaggedDependsNonTagged()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("inline-nontagged-depends-nontagged-ok.qml"));
    QCOMPARE(results().passCount(), 4);
}

void test_precedence::inlineNontaggedDependsSingleTagged()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("inline-nontagged-depends-single-tagged-ok.qml"));
    QCOMPARE(results().passCount(), 4);
}

void test_precedence::inlineNontaggedDependsMultipleTagged()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("inline-nontagged-depends-multiple-tagged-ok.qml"));
    QCOMPARE(results().passCount(), 3);
}

void test_precedence::inlineNontaggedDependsNontaggedWrongName()
{
    RUN_AND_EXPECT(qst::ExitApplicationError, "-f", dataPath("inline-wrong-name.qml"));
    QVERIFY(stdError().contains("inline-wrong-name.qml:11"));
}


void test_precedence::inlineTaggedDependsNontagged()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("inline-tagged-depends-nontagged-ok.qml"));
    QCOMPARE(results().passCount(), 4);
    VERIFY_EXECUTION_ORDER({"level1", "level2 0000000 [ tag1 ]"});
    VERIFY_EXECUTION_ORDER({"level1", "level2 0000001 [ tag2 ]"});
    VERIFY_EXECUTION_ORDER({"level1", "level2 0000002 [ tag3 ]"});
}

void test_precedence::inlineTaggedDependsSingleTagged()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("inline-tagged-depends-single-tagged-ok.qml"));
    QCOMPARE(results().passCount(), 4);
    VERIFY_EXECUTION_ORDER({"level1 0000000 [ tag1 ]", "level2 0000002 [ tag1 ]"});
    VERIFY_EXECUTION_ORDER({"level1 0000001 [ tag2 ]", "level2 0000003 [ tag2 ]"});
}

void test_precedence::inlineTaggedDependsMultipleTagged()
{
    QSKIP("not implemented");
}


void test_precedence::inlineNontaggedExportsIllegalName()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("inline-nontagged-exports-illegal-name.qml"));
}

void test_precedence::inlineCycle()
{
    RUN_AND_EXPECT(qst::ExitApplicationError, "-f", dataPath("inline-cycle.qml"));
    QVERIFY(stdError().contains("inline-cycle.qml:6"));
    QVERIFY(stdError().contains("inline-cycle.qml:12"));
}

QTEST_MAIN(test_precedence)

#include "test-precedence.moc"

