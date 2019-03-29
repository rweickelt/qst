/****************************************************************************
 **
 ** Copyright (C) 2019 The Qst project.
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
#include <qsttest.h>

class test_resource : public QstTest {
    Q_OBJECT
public:
    QString dataPath(const QString& fileName) const;

private slots:
    //    void inlineNontaggedDependsNonTaggedOk();
    void dependsSingleResourcePerName();
    void duplicateNames();
};

QString test_resource::dataPath(const QString& fileName) const
{
    return QDir(QString(SOURCE_DIR)).absoluteFilePath(fileName);
}

void test_resource::dependsSingleResourcePerName()
{
    RUN_AND_EXPECT(qst::ExitNormal, "-f", dataPath("depends-single-resource-per-name.qml"));
    QCOMPARE(results().passCount(), 1);
}

void test_resource::duplicateNames()
{
    RUN_AND_EXPECT(qst::ExitApplicationError, "-f", dataPath("duplicate-names.qml"));
    QVERIFY(stdError().contains("duplicate-names.qml:4"));
}

QTEST_MAIN(test_resource)

#include "test-resource.moc"
