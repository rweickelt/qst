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
#include "jobdispatcher.h"
#include "project.h"
#include "qst.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtQml/QQmlProperty>

#include <QtDebug>

JobDispatcher::JobDispatcher(Project* project, const TagLookupTable& tags)
    : m_project(project), m_tags(tags)
{
    createProjectWorkingDirectory();
}

void JobDispatcher::dispatch(const Job& job)
{
    QString name = job.testcase()->name();
    QString displayName = name;
    QString workingDirectoryName = name;

    if (job.tagGroupId() != InvalidId)
    {
        const auto& taglist = m_tags[job.tagGroupId()][job.tagId()];

        displayName = QString("%1 %2 [ %3 ]")
                .arg(name)
                .arg(job.tagId(), 7, 36, QChar('0'))
                .arg(QVariant(taglist.values()).toStringList().join(" "));

        workingDirectoryName = QString("%1-%2")
                .arg(name)
                .arg(job.tagId(), 7, 36, QChar('0'));

        for (const auto& key: taglist.keys())
        {
            QQmlProperty property(job.testcase(), key);
            Q_ASSERT(property.isProperty());
            Q_ASSERT(property.isWritable());
            property.write(taglist[key]);
        }
    }

    QString workingDirectory = createTestcaseWorkingDirectory(workingDirectoryName);
    job.testcase()->setWorkingDirectory(workingDirectory);
    job.testcase()->setDisplayName(displayName);

    m_results << job.testcase()->exec();

    emit finished(job);
}

void JobDispatcher::createProjectWorkingDirectory()
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

QString JobDispatcher::createTestcaseWorkingDirectory(const QString& name)
{
    QDir projectWorkDir(m_project->workingDirectory());
    QDir testcaseWorkDir(projectWorkDir.absoluteFilePath(name));

    if (testcaseWorkDir.exists())
    {
        if (!testcaseWorkDir.removeRecursively())
        {
            QST_ERROR_AND_EXIT(QString("Could not wipe directory '%1'.")
                                .arg(testcaseWorkDir.absolutePath()));
        }
    }

    if ((!projectWorkDir.mkdir(name)))
    {
        QST_ERROR_AND_EXIT(QString("Could not create working directory '%1'.")
                            .arg(testcaseWorkDir.absolutePath()));
    }

    return testcaseWorkDir.absolutePath();
}

