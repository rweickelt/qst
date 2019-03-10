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

#ifndef RESOURCE_H
#define RESOURCE_H

#include "qstitem.h"

class Resource : public QstItem {
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER m_name READ name)

public:
    Resource(QObject* parent = nullptr);
    virtual const QMetaObject* baseTypeInfo() const final;
    QString name() const;

protected:
    virtual void callVisitor(QstItemVisitor* visitor) final;
    virtual void handleParserEvent(ParserEvent event) final { Q_UNUSED(event); }

private:
    QString m_type;
    QString m_name;
};

inline const QMetaObject* Resource::baseTypeInfo() const { return &Resource::staticMetaObject; }
inline QString Resource::name() const { return m_name; }

#endif // RESOURCE_H
