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

#ifndef EXEMPTION_H
#define EXEMPTION_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QVariantMap>
#include <QtCore/QVector>

#include <QtQml/private/qqmlcustomparser_p.h>

#include "qstitem.h"

class Dimension : public QstItem
{
    Q_OBJECT
    Q_PROPERTY(QStringList testcases READ testcases WRITE setTestcases NOTIFY testcasesChanged)

    friend class DimensionParser;

signals:
    void testcasesChanged();

public:
    Dimension(QObject* parent = nullptr);
    virtual const QMetaObject* baseTypeInfo() const final;
    virtual void callVisitor(QstItemVisitor* visitor) final;
    const QVector<QVariantMap>& data() const;
    int length() const;
    QStringList testcases() const;
    void setTestcases(const QStringList& names);

protected:
    void handleParserEvent(QstItem::ParserEvent event) override;
    void setData(const QVector<QVariantMap>& data);

private:

    QStringList m_testcases;
    QVector<QVariantMap> m_data;
};

inline const QMetaObject* Dimension::baseTypeInfo() const { return &Dimension::staticMetaObject; }
inline QStringList Dimension::testcases() const { return m_testcases; }

class DimensionParser : public QQmlCustomParser
{
public:
    void verifyBindings(const QV4::CompiledData::Unit* qmlUnit, const QList<const QV4::CompiledData::Binding*>& props) override;
    void applyBindings(QObject* object, QV4::CompiledData::CompilationUnit* compilationUnit, const QList<const QV4::CompiledData::Binding*> &bindings) override;
private:
    bool definesEmptyList(const QString& script);
};


#endif // EXEMPTION_H
