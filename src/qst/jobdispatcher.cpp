/****************************************************************************
 **
 ** Copyright (C) 2017-2019 The Qst project.
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
#include "projectdatabase.h"
#include "tag.h"
#include "testcase.h"
#include "qst.h"

#include <QtCore/QFileInfo>
#include <QtQml/QQmlProperty>

JobDispatcher::JobDispatcher(const ProjectDatabase& database)
    : m_db(database)
{
    m_projectWorkingDirectory.setPath(m_db.project["workingDirectory"].toString());
}

void JobDispatcher::dispatch(Job job)
{
    QString name = job.name();
    QString displayName = name;
    QString workingDirectoryName = name;
    QList<Tag> tags = job.tags().toList();
    std::sort(tags.begin(), tags.end());

    if (job.tags().size() > 0)
    {
        QStringList tagsStrings;
        for (const auto& tag: tags)
        {
            tagsStrings << tag.value();

            QQmlProperty property(job.testcase(), tag.label());
            Q_ASSERT(property.isProperty());
            Q_ASSERT(property.isWritable());
            property.write(tag.value());
        }

        displayName = QString("%1 %2 [ %3 ]")
                .arg(name)
                .arg(job.id(), 7, 36, QChar('0'))
                .arg(tagsStrings.join(" "));

        workingDirectoryName = QString("%1-%2")
                .arg(name)
                .arg(job.id(), 7, 36, QChar('0'));
    }

    QString workingDirectory = createTestcaseWorkingDirectory(workingDirectoryName);
    job.testcase()->setWorkingDirectory(workingDirectory);
    job.testcase()->setDisplayName(displayName);

    Testcase::Result result = job.testcase()->exec();
    job.setResult(result);

    emit finished(job);
}

QString JobDispatcher::createTestcaseWorkingDirectory(const QString& name)
{
    QString workDirPath = m_projectWorkingDirectory.absolutePath();
    if (!QFileInfo::exists(workDirPath))
    {
        if (!QDir().mkpath(workDirPath))
        {
            QST_ERROR_AND_EXIT(QString("Could not create working directory '%1'.").arg(workDirPath));
        }
    }
    else if (!QFileInfo(workDirPath).isDir())
    {
        QST_ERROR_AND_EXIT(QString("Working directory path '%1' is not a valid directory.").arg(workDirPath));
    }
    else
    {
        if (!QFileInfo(workDirPath).isWritable())
        {
            QST_ERROR_AND_EXIT(QString("Working directory '%1' is not writable.").arg(workDirPath));
        }
    }

    QDir testcaseWorkDir(m_projectWorkingDirectory.absoluteFilePath(name));

    if (testcaseWorkDir.exists())
    {
        if (!testcaseWorkDir.removeRecursively())
        {
            QST_ERROR_AND_EXIT(QString("Could not wipe directory '%1'.")
                                .arg(testcaseWorkDir.absolutePath()));
        }
    }

    if ((!m_projectWorkingDirectory.mkdir(name)))
    {
        QST_ERROR_AND_EXIT(QString("Could not create working directory '%1'.")
                            .arg(testcaseWorkDir.absolutePath()));
    }

    return testcaseWorkDir.absolutePath();
}

