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
#include "testcase.h"
#include <QtDebug>

Component::Component(QObject *parent) : QObject(parent)
{
}

QQmlListProperty<QObject> Component::defaultProperty()
{
    return QQmlListProperty<QObject>(this, m_defaultProperty);
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

void Component::componentComplete()
{
    testCase()->registerChild(this);
}
