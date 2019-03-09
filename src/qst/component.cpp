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
#include "qst.h"
#include "qstitemvisitor.h"
#include "testcase.h"

#include <QtCore/QThreadStorage>
#include <QtDebug>

namespace {
    QThreadStorage<QHash<QString, int> > instancesCount;
}

Component::Component(QObject* parent) : QstItem(parent)
{
    // Alias signal
    QObject::connect(this, &QObject::objectNameChanged, this, &Component::nameChanged);
}

void Component::callVisitor(QstItemVisitor* visitor)
{
    visitor->visit(this);
}

QString Component::filepath() const
{
    return qst::qmlDefinitionContext(this).file();
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

const Testcase* Component::testCase() const
{
    return const_cast<Testcase*>(testCase());
}

void Component::handleParserEvent(QstItem::ParserEvent event)
{
    switch (event)
    {
    case AfterClassBegin:
    {
        if (name().isEmpty())
        {
            QString typeName = this->metaObject()->className();
            int pos = typeName.indexOf("_QML_");
            if (pos > 0)
            {
                typeName = typeName.left(pos);
            }
            int count = instancesCount.localData()[typeName]++;
            setObjectName(QString("%1-%2").arg(typeName.toLower()).arg(count));
        }
    }
        break;
    default:
        break;
    }
}

void Component::resetInstancesCounter()
{
    instancesCount.localData().clear();
}
