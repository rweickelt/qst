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

#ifndef DEPENDS_H
#define DEPENDS_H

#include "qstitem.h"
#include "tag.h"

#include <QtCore/QList>

class Depends : public QstItem
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    Depends(QObject* parent = nullptr);

    void accept(QstItemVisitor* visitor);
    virtual const QMetaObject* baseTypeInfo() const final;
    QString name() const;
    void setName(const QString& name);
    QList<Tag> tags() const;

signals:
    void nameChanged();

protected:
    virtual void callVisitor(QstItemVisitor* visitor) final;
    virtual void handleParserEvent(ParserEvent event) final { Q_UNUSED(event); }

private:
    QString m_name;
};

inline const QMetaObject* Depends::baseTypeInfo() const { return &Depends::staticMetaObject; }
inline QString Depends::name() const { return m_name; }
#endif // DEPENDS_H
