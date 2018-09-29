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
    void inlineNontaggedDependsNonTaggedOk();
    void inlineNontaggedDependsSingleTaggedOk();
    void inlineNontaggedDependsMultipleTaggedOk();
    void inlineNontaggedDependsNontaggedWrongNameOk();
    void inlineTaggedDependsNontaggedOk();
    void inlineTaggedDependsSingleMatchingTaggedOk();
    void inlineTaggedDependsMultipleTaggedOk();
    void dependsIllegalNameOrAliasError();
    void inlineCycleError();
};

QString test_precedence::dataPath(const QString &fileName) const
{
    return QDir(QString(SOURCE_DIR)).absoluteFilePath(fileName);
}

void test_precedence::inlineNontaggedDependsNonTaggedOk()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("inline-nontagged-depends-nontagged-ok.qml"));
    QCOMPARE(results().passCount(), 4);
}

void test_precedence::inlineNontaggedDependsSingleTaggedOk()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("inline-nontagged-depends-single-tagged-ok.qml"));
    QCOMPARE(results().passCount(), 4);
}

void test_precedence::inlineNontaggedDependsMultipleTaggedOk()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("inline-nontagged-depends-multiple-tagged-ok.qml"));
    QCOMPARE(results().passCount(), 4);
}

void test_precedence::inlineNontaggedDependsNontaggedWrongNameOk()
{
    RUN_AND_EXPECT(qst::ExitApplicationError, "-f", dataPath("inline-wrong-name.qml"));
    QVERIFY(stdError().contains("inline-wrong-name.qml:11"));
}


void test_precedence::inlineTaggedDependsNontaggedOk()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("inline-tagged-depends-nontagged-ok.qml"));
    QCOMPARE(results().passCount(), 4);
    VERIFY_EXECUTION_ORDER({"level1", "level2 0000000 [ tag1 ]"});
    VERIFY_EXECUTION_ORDER({"level1", "level2 0000001 [ tag2 ]"});
    VERIFY_EXECUTION_ORDER({"level1", "level2 0000002 [ tag3 ]"});
}

void test_precedence::inlineTaggedDependsSingleMatchingTaggedOk()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("inline-tagged-depends-single-matching-tagged-ok.qml"));
    QCOMPARE(results().passCount(), 4);
    VERIFY_EXECUTION_ORDER({"level1 0000000 [ tag1 ]", "level2 0000002 [ tag1 ]"});
    VERIFY_EXECUTION_ORDER({"level1 0000001 [ tag2 ]", "level2 0000003 [ tag2 ]"});
}

void test_precedence::inlineTaggedDependsMultipleTaggedOk()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("inline-tagged-depends-single-matching-tagged-ok.qml"));
}


void test_precedence::dependsIllegalNameOrAliasError()
{
    RUN_AND_EXPECT(qst::ExitApplicationError, "-f", dataPath("depends-illegal-name-or-alias.qml"));
    QVERIFY(stdError().contains("depends-illegal-name-or-alias.qml:12:"));
    QVERIFY(stdError().contains("depends-illegal-name-or-alias.qml:18:"));
    QVERIFY(stdError().contains("depends-illegal-name-or-alias.qml:24:"));
    QVERIFY(stdError().contains("depends-illegal-name-or-alias.qml:31:"));
    QVERIFY(stdError().contains("depends-illegal-name-or-alias.qml:38:"));
    QVERIFY(stdError().contains("depends-illegal-name-or-alias.qml:44:"));
    QVERIFY(stdError().contains("depends-illegal-name-or-alias.qml:50:"));
    QVERIFY(stdError().contains("depends-illegal-name-or-alias.qml:56:"));
    QVERIFY(stdError().contains("depends-illegal-name-or-alias.qml:63:"));
}

void test_precedence::inlineCycleError()
{
    RUN_AND_EXPECT(qst::ExitApplicationError, "-f", dataPath("inline-cycle.qml"));
    QVERIFY(stdError().contains("inline-cycle.qml:12"));
    QVERIFY(stdError().contains("inline-cycle.qml:18"));
}

QTEST_MAIN(test_precedence)

#include "test-precedence.moc"

