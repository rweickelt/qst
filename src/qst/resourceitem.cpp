/****************************************************************************
 **
 ** Copyright (C) 2019 The Qst project.
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

#include "resourceitem.h"
#include "qstitemvisitor.h"

#include <QtCore/QMetaProperty>

ResourceItem::ResourceItem(QObject* parent)
    : QstItem(parent)
{
    QObject::connect(this, &QObject::objectNameChanged, this, &ResourceItem::nameChanged);
}

QVariantMap ResourceItem::data() const
{
    QVariantMap result;

    const static QStringList ignoredProperties{ "objectName", "nestedComponents", "name" };
    const QMetaObject* metaobject = this->metaObject();
    int count = metaobject->propertyCount();
    for (int i = 0; i < count; i++)
    {
        QMetaProperty metaproperty = metaobject->property(i);
        const char* name = metaproperty.name();

        if (ignoredProperties.contains(QLatin1String(name)) || (!metaproperty.isReadable()))
        {
            continue;
        }

        QVariant value = this->property(name);
        result[QLatin1String(name)] = value;
    }
    return result;
}

void ResourceItem::callVisitor(QstItemVisitor* visitor)
{
    visitor->visit(this);
}
