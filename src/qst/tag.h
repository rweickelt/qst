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

#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtCore/QString>

class Tag {
public:
    using Id = int;

    Id id() const;
    QString label() const;
    QString value() const;

    static Tag create(const QString& label, const QString& value);
    QPair<QString, QString> toPair() const;
    QString toString() const;

    bool operator==(const Tag& other) const;

private:
    Id m_id = -1;
};

uint qHash(const Tag& tag);

inline Tag::Id Tag::id() const { return m_id; }
inline bool Tag::operator==(const Tag& other) const { return m_id == other.m_id; }
inline uint qHash(const Tag& tag) { return tag.id(); }

using TagList = QList<Tag>;

#endif // TAG_H
