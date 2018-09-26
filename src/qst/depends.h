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
#include <QtQml/private/qqmlcustomparser_p.h>


class Depends : public QstItem
{
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER m_name)
    Q_PROPERTY(QString alias MEMBER m_alias)

public:
    Depends(QObject* parent = nullptr);

    virtual const QMetaObject* baseTypeInfo() const final;
    void accept(QstItemVisitor* visitor);

    QString alias() const;
    QString name() const;
    QList<TagSet> tags() const;

protected:
    virtual void callVisitor(QstItemVisitor* visitor) final;
    virtual void handleParserEvent(ParserEvent event) final;

private:
    friend class DependsParser;

    QString m_alias;
    QString m_name;
    QList<TagSet> m_tags;
};

inline const QMetaObject* Depends::baseTypeInfo() const { return &Depends::staticMetaObject; }
inline QString Depends::alias() const { return m_alias; }
inline QString Depends::name() const { return m_name; }
inline QList<TagSet> Depends::tags() const { return m_tags; }

class DependsParser : public QQmlCustomParser
{
public:
    void verifyBindings(const QV4::CompiledData::Unit* qmlUnit, const QList<const QV4::CompiledData::Binding*>& props) override;
    void applyBindings(QObject* object, QV4::CompiledData::CompilationUnit* compilationUnit, const QList<const QV4::CompiledData::Binding*> &bindings) override;
private:
    bool definesEmptyList(const QString& script);
};

#endif // DEPENDS_H
