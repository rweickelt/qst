/****************************************************************************
 **
 ** Copyright (C) 2017, 2018 The Qst project.
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
#ifndef TESTCASEITEM_H
#define TESTCASEITEM_H

#include "qstitem.h"

#include <QtCore/QString>

class ProjectResolver;
class Testcase;

class Component : public QstItem
{
    Q_OBJECT
    // Alias for objectName
    Q_PROPERTY(QString name READ name WRITE setObjectName NOTIFY nameChanged)

    friend class ProjectResolver;
    friend class Testcase;

public:
    Component(QObject* parent = nullptr);
    virtual const QMetaObject* baseTypeInfo() const override;

    QString name() const;
    QString filepath() const;
    Testcase* testCase();
    const Testcase* testCase() const;
    static void resetInstancesCounter();

signals:
    void nameChanged(const QString &objectName);

protected:
    virtual void callVisitor(QstItemVisitor* visitor) override;
    virtual void initTestCase() {}
    virtual void initTestFunction() {}
    virtual void cleanupTestCase() {}
    virtual void cleanupTestFunction() {}

    virtual void handleParserEvent(QstItem::ParserEvent event) override;
};

inline const QMetaObject* Component::baseTypeInfo() const { return &Component::staticMetaObject; }
inline QString Component::name() const { return objectName(); }

#endif // TESTCASEITEM_H
