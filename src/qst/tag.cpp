/****************************************************************************
 **
 ** Copyright (C) 2018 The Qst project.
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

#include "tag.h"

#include <QtCore/QMap>
#include <QtCore/QVector>

namespace {
    QVector<QString> stringsById;
    QHash<QString, qint32> idByStrings;
}

Tag Tag::create(const QString& label, const QString& value)
{
    qint64 labelId = idByStrings.value(label, -1);
    if (labelId < 0)
    {
        stringsById.append(label);
        labelId = stringsById.size() - 1;
        idByStrings.insert(label, labelId);
    }

    qint64 valueId = idByStrings.value(value, -1);
    if (valueId < 0)
    {
        stringsById.append(value);
        valueId = stringsById.size() - 1;
        idByStrings.insert(value, valueId);
    }

    Tag tag;
    tag.m_labelId = labelId;
    tag.m_valueId = valueId;

    return tag;
}

QString Tag::label() const
{
    return stringsById[m_labelId];
}

QString Tag::value() const
{
    return stringsById[m_valueId];
}

bool Tag::operator<(const Tag& other) const
{
    return stringsById[m_labelId] < stringsById[other.m_labelId];
}

bool Tag::operator==(const Tag& other) const
{
    return (m_labelId == other.m_labelId)
            && (m_valueId == other.m_valueId);
}

uint qHash(const Tag& tag)
{
    Q_ASSERT(tag.m_labelId >= 0);
    Q_ASSERT(tag.m_valueId >= 0);

    return tag.m_labelId + (tag.m_valueId << 16);

}

