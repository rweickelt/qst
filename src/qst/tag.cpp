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

namespace {
    QMap<Tag::Id, QPair<QString, QString> > stringTable;
}

Tag Tag::create(const QString& label, const QString& value)
{
    Tag::Id id = qHash(label) + qHash(value);
    if (stringTable.contains(id))
    {
        const QPair<QString, QString>& other = stringTable[id];
        // If this fails, we have to come up with a more clever storage and look-up.
        Q_ASSERT((other.first == label) && (other.second == value));
    }
    stringTable[id] = { label, value };

    Tag tag;
    tag.m_id = id;
    return tag;
}

QString Tag::label() const
{
    return stringTable[m_id].first;
}

QString Tag::value() const
{
    return stringTable[m_id].second;
}

QPair<QString, QString> Tag::toPair() const
{
    return stringTable[m_id];
}

QString Tag::toString() const
{
    QPair<QString, QString> tag = stringTable[m_id];
    return QString("%1:%2").arg(tag.first).arg(tag.second);
}

