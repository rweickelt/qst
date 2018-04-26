/****************************************************************************
 **
 ** Copyright (C) 2017-2018 The Qst project.
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
#include "matrix.h"
#include "project.h"
#include "qst.h"
#include "projectresolver.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QHash>

#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>

Project::Project(QObject *parent) : QstItem(&Project::staticMetaObject, parent)
{
    QstItem::setAllowedParentTypes({nullptr});
    QstItem::setAllowedNestedTypes({&Matrix::staticMetaObject, &Testcase::staticMetaObject});
}

void Project::handleParserEvent(ParserEvent event)
{
    if (event == AfterClassBegin)
    {
        if (m_workingDirectory.isEmpty())
        {
            QString profileName = qmlEngine(this)->rootContext()->contextProperty("profile").toMap().value("name").toString();
            if (profileName.isEmpty())
            {
                profileName = "default";
            }
            QString workDirName = QString(".%1-%2-%3")
                    .arg(m_name)
                    .arg(profileName)
                    .arg(qHash(m_filepath), 0, 16);
            m_workingDirectory = QDir().absoluteFilePath(workDirName);
        }
    }
    else if (event == ClassBegin)
    {
        ApplicationOptions* options = ApplicationOptions::instance();
        m_workingDirectory = options->workingDirectory;
        m_name = "project";
        m_filepath = ProjectResolver::instance()->currentDocument()->filepath;
    }
}
