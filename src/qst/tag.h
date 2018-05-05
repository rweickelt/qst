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

#ifndef TAG_H
#define TAG_H

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QVariantMap>

typedef QVariantMap Tag;
typedef uint TagGroupId; // Hash of tag names (property names)
typedef uint TagId;      // Hash of tag values (property values)
typedef QMap<TagGroupId, QMap<TagId, Tag> > TagLookupTable;

enum {
    InvalidId = static_cast<uint>(-1)
};

TagGroupId makeTagGroupId(const Tag& tag);
TagId makeTagId(const Tag& tag);

#endif // TAG_H
