/****************************************************************************
 **
 ** Copyright (C) 2018-2019 The Qst project.
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
#ifndef MATRIX_H
#define MATRIX_H

#include "qstitem.h"

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtQml/QQmlListProperty>

class Dimension;

class Matrix : public QstItem
{
    Q_OBJECT
    Q_PROPERTY(QStringList names READ names WRITE setTestcases NOTIFY testcasesChanged)

signals:
    void testcasesChanged();

public:
    enum Scope {
        ProjectScope,
        TestcaseScope
    };

    Matrix(QObject* parent = nullptr);
    virtual const QMetaObject* baseTypeInfo() const final;
    QList<Dimension*> dimensions() const;
    QStringList names() const;
    Scope scope() const;
    void setTestcases(const QStringList& names);

protected:
    virtual void callVisitor(QstItemVisitor* visitor) final;
    void handleParserEvent(QstItem::ParserEvent event);

private:
    Scope m_scope;
    QStringList m_names;
};

inline const QMetaObject* Matrix::baseTypeInfo() const { return &Matrix::staticMetaObject; }
inline Matrix::Scope Matrix::scope() const { return m_scope; }
inline QStringList Matrix::names() const { return m_names; }

#endif // MATRIX_H
