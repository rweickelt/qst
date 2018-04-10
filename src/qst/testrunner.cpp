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
#include "testrunner.h"
#include "project.h"
#include "qst.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>

TestRunner::TestRunner(Project* project, const QList<QPointer<Testcase> >& testCases):
    m_project(project)
{
    m_testCases = testCases;
    createWorkingDirectory();
}

void TestRunner::execTestCases()
{
    for (int index = 0; index < m_testCases.size(); index++)
    {
        m_results << m_testCases[index]->exec();
    }

    if (m_results.contains(Testcase::Fail))
    {
        QCoreApplication::exit(qst::ExitTestCaseFailed);
    }
    else
    {
        QCoreApplication::exit(qst::ExitNormal);
    }
}

void TestRunner::createWorkingDirectory()
{
    QString workDirPath = m_project->workingDirectory();

    if (!QFileInfo(workDirPath).exists())
    {
        if (!QDir().mkpath(workDirPath))
        {
            m_errorString = QString("Could not create working directory '%1'.").arg(workDirPath);
            return;
        }
    }
    else if (!QFileInfo(workDirPath).isDir())
    {
        m_errorString = QString("Working directory path '%1' is not a valid directory.").arg(workDirPath);
        return;
    }
    else
    {
        if (!QFileInfo(workDirPath).isWritable())
        {
            m_errorString = QString("Working directory '%1' is not writable.").arg(workDirPath);
            return;
        }
    }
}
