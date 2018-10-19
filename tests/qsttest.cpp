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
#include "qsttest.h"
#include "qsttestresults.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QProcessEnvironment>
#include <QtTest/QTest>

#ifndef PROJECTPATH
#error PROJECTPATH is not defined.
#endif

const QDir QstTest::m_dataDirectory =
        QDir(QProcessEnvironment::systemEnvironment().value(
                 "PROJECTPATH", PROJECTPATH) + "/tests/testdata/");

const QString QstTest::m_defaultImportPath =
        QDir(QProcessEnvironment::systemEnvironment().value(
                 "PROJECTPATH", PROJECTPATH) + "/share/qst/imports/").absolutePath();

QstTest::QstTest()
{
    m_qstProcess.setWorkingDirectory(QDir::tempPath());
}

QString QstTest::dataPath(const QString& directory) const
{
    return m_dataDirectory.absoluteFilePath(directory);
}

QstTestResults QstTest::execQstRun(const QStringList& arguments, int timeoutMs)
{
    QStringList cmdLine = { "run", "--import", m_defaultImportPath };
    cmdLine += arguments;
    QDir qstDir(QCoreApplication::applicationDirPath());
    m_qstProcess.start(qstDir.absoluteFilePath("qst"), cmdLine);
    if (!m_qstProcess.waitForFinished(timeoutMs))
    {
        m_qstProcess.kill();
    }

    return QstTestResults::fromQstOutput(m_qstProcess.readAllStandardOutput());
}


bool QstTest::execQstRun(const QStringList& arguments, int expectedExitCode, const QString& file, int line)
{
    QStringList cmdLine = { "run", "--import", m_defaultImportPath };
    cmdLine += arguments;
    QDir qstDir(QCoreApplication::applicationDirPath());
    m_qstProcess.start(qstDir.absoluteFilePath("qst"), cmdLine);
    m_qstProcess.waitForFinished();

    if (m_qstProcess.exitCode() == expectedExitCode)
    {
        if (expectedExitCode == qst::ExitApplicationError)
        {
            m_stdError = m_qstProcess.readAllStandardError();
        }
        else
        {
            m_results = QstTestResults::fromQstOutput(m_qstProcess.readAllStandardOutput());
        }
        return true;
    }

    QStringList log = QString(m_qstProcess.readAllStandardOutput()).split('\n');
    for (auto& logline: log)
    {
        logline.prepend("    ");
    }

    QString statement = QString("%1 ended with wrong result. Expected: %2, actual: %3")
            .arg("qst " + m_qstProcess.arguments().join(' '))
            .arg(expectedExitCode)
            .arg(m_qstProcess.exitCode());

    m_stdError = m_qstProcess.readAllStandardError();

    QString description = QString("\n%1%2")
            .arg(log.join('\n'))
            .arg(stdError());

    QTest::qVerify(false, qPrintable(statement), qPrintable(description), qPrintable(file), line);

    return false;
}

bool QstTest::verifyExecutionOrder(const QStringList& expected, const QStringList& actual)
{
    if (actual.length() < expected.length())
        return false;

    if (actual.length() == 0)
        return false;

    if (expected.length() == 1)
        return actual.contains(expected.first());

    for (int i = 1; i < expected.length(); i++)
        return (actual.indexOf(expected[i-1]) < actual.indexOf(expected[i]));

    return false;
}
