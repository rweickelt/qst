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
#include "component.h"
#include "projectresolver.h"
#include "testcase.h"
#include <QtDebug>

namespace {
    QHash<QString, int> instancesCount;
}

Component::Component(QObject *parent) : QObject(parent)
{
}

Testcase* Component::testCase()
{
    for (QObject* currentObject = this; currentObject != NULL;
         currentObject = currentObject->parent())
    {
        if (currentObject->metaObject()->inherits(&Testcase::staticMetaObject))
        {
            return qobject_cast<Testcase*>(currentObject);
        }
    }
    Q_ASSERT(false);
    return nullptr;
}

void Component::setName(const QString& name)
{
    if (name != m_name)
    {
        m_name = name;
        emit nameChanged();
    }
}

void Component::handleParserEvent(ParserEventHandler::ParserEvent event)
{
    switch (event)
    {
    case AfterClassBegin:
    {
        m_typeName = this->metaObject()->className();
        int pos = m_typeName.indexOf("_QML_");
        if (pos > 0)
        {
            m_typeName = m_typeName.left(pos);
        }
        if (m_name.isEmpty())
        {
            int count = instancesCount[m_typeName]++;
            m_name = QString("%1-%2").arg(m_typeName.toLower()).arg(count);
        }
        this->setObjectName(m_name);
    }
        break;
    case ClassBegin:
        m_filepath = ProjectResolver::instance()->currentDocument()->filepath;
        ProjectResolver::instance()->currentDocument()->components.append(this);
        break;
    default:
        break;
    }
}

