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
#include "applicationoptions.h"
#include "console.h"
#include "project.h"
#include "qst.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QHash>
#include <QtQml/private/qqmlcontext_p.h>

Project::Project(QObject *parent) : QObject(parent)
{

}


void Project::classBegin()
{
    ApplicationOptions* options = ApplicationOptions::instance();
    m_workingDirectory = options->workingDirectory;
    m_name = "qst-project";
}


void Project::componentComplete()
{
    QString workDirPath = m_workingDirectory;
    QDir workDir(workDirPath);

    if (!workDirPath.isEmpty())
    {
        if (!workDir.exists())
        {
            if (!QDir().mkpath(workDirPath))
            {
                Console::printError(QString("Could not create working directory '%1'.")
                                    .arg(workDirPath));
                ::exit(qst::ExitApplicationError);
            }
        }
        else if (!QFileInfo(workDirPath).isDir())
        {
            Console::printError(QString("Value of --working-directory is not a valid directory."));
            ::exit(qst::ExitApplicationError);
        }
        else
        {
            if (!QFileInfo(workDirPath).isWritable())
            {
                Console::printError(QString("Working directory is not writable."));
                ::exit(qst::ExitApplicationError);
            }
        }
    }
    else
    {
        QQmlData* data = QQmlData::get(this, false);
        // Hash of project name should be replaced by profile name
        QString workDirName = QString("%1-%2")
                .arg(m_name)
                .arg(QString::number(qHash(data->outerContext->url().path()), 16));
        workDirPath = QDir(QDir::temp()).filePath(workDirName);
        if (!QDir(workDirPath).exists() && !QDir(QDir::temp()).mkdir(workDirName))
        {
            Console::printError(QString("Could not create temporary directory '%1'.")
                                .arg(workDirPath));
            ::exit(qst::ExitApplicationError);
        }
        m_workingDirectory = workDirPath;
    }
}
