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
#include "jobrunner.h"
#include "project.h"
#include "qst.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtQml/QQmlProperty>

#include <QtDebug>

JobRunner::JobRunner(Project* project, const QList<TestJob>& jobs,
                     const QVector<QVariantMap>& tags):
    m_project(project), m_jobs(jobs), m_tags(tags)
{
    createProjectWorkingDirectory();
}

void JobRunner::execTestCases()
{
    for (auto& job: m_jobs)
    {
        if (job.tagEntry >= 0) {
            QString name = job.testcase->name();
            job.testcase->setName(QString("%1-%2").arg(name).arg(job.tagEntry));
            const auto& values = m_tags[job.tagEntry];
            for (const auto& key: values.keys())
            {
                QQmlProperty property(job.testcase, key);
//                Q_ASSERT(property.isProperty());
//                Q_ASSERT(property.isWritable());
                property.write(values[key]);
            }
            m_results << job.testcase->exec();
            job.testcase->setName(name);
        }
        else
        {
            m_results << job.testcase->exec();
        }
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

void JobRunner::createProjectWorkingDirectory()
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
