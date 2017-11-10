/****************************************************************************
 **
 ** Copyright (C) 2017 The Qst project.
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

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QString>

#include <QtQml/QQmlListProperty>
#include <QtQml/QQmlParserStatus>

class TestCase;

class Component : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_DISABLE_COPY(Component)
    Q_CLASSINFO("DefaultProperty", "children")

    friend class TestCase;

    Q_PROPERTY(QQmlListProperty<QObject> children READ children CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName)

public:
    explicit Component(QObject* parent = 0);
    QQmlListProperty<QObject> children();
    TestCase* testCase();

    template <typename T>
    QList<T*> childrenByType() const;
    QString name() const;
    QString filepath() const;
    void setName(const QString& name);
    void setFilepath(const QString& filepath);


protected:
    virtual void initTestCase() {}
    virtual void initTestFunction() {}
    virtual void cleanupTestCase() {}
    virtual void cleanupTestFunction() {}
    virtual void classBegin() {}
    virtual void componentComplete() {}

    QList<QObject *> m_children;
    QString m_name;
    QString m_filepath;
};

template <typename T>
QList<T*> Component::childrenByType() const
{
    QList<T*> result;
    for (auto child : m_children)
    {
        if (child->metaObject()->inherits(&T::staticMetaObject))
        {
            result << qobject_cast<T*>(child);
            if (m_children.size() > 0)
            {
                result << qobject_cast<T*>(child)->childrenByType<T>();
            }
        }
    }
    return result;
}

inline QString Component::filepath() const { return m_filepath; }
inline QString Component::name() const { return m_name; }

inline void Component::setFilepath(const QString& filepath) { m_filepath = filepath; }
inline void Component::setName(const QString& name) { m_name = name; }

#endif // TESTCASEITEM_H
