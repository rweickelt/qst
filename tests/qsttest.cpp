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
#include "qsttest.h"
#include "qsttestresults.h"

#include <QtCore/QCoreApplication>
#include <QtTest/QTest>

#ifndef PROJECTPATH
#error PROJECTPATH is not defined.
#endif

QString QstTest::dataPath(const QString& directory) const
{
    return QDir(PROJECTPATH "/tests/testdata/").absoluteFilePath(directory);
}

QString QstTest::defaultImportPath() const
{
    return QDir(PROJECTPATH "/share/qst/imports/").absolutePath();
}

QstTestResults QstTest::execQstRun(const QStringList& arguments)
{
    QStringList cmdLine = { "run", "--import", defaultImportPath() };
    cmdLine += arguments;
    QDir qstDir(QCoreApplication::applicationDirPath());
    m_qstProcess.start(qstDir.absoluteFilePath("qst"), cmdLine);
    if (!m_qstProcess.waitForFinished(500))
    {
        m_qstProcess.kill();
    }

    return QstTestResults::fromQstOutput(m_qstProcess.readAllStandardOutput());
}
