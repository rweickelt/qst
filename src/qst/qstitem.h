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

#ifndef PARSERSTATEHANDLER_H
#define PARSERSTATEHANDLER_H

#include "qstitemvisitor.h"

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtQml/QQmlListProperty>
#include <QtQml/QQmlParserStatus>

class QstItem : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(QQmlListProperty<QObject> nestedComponents READ nested)
    Q_CLASSINFO("DefaultProperty", "nestedComponents")

    friend class ProjectResolver;
    friend class JobDispatcher;
    friend class QstItemVisitor;

public:
    QstItem(QObject* parent = nullptr);
    void accept(QstItemVisitor* visitor);
    virtual const QMetaObject* baseTypeInfo() const = 0;

protected:
    enum ParserEvent {
        AfterClassBegin,
        AfterComponentComplete,
        ClassBegin,
        ComponentComplete
    };

    virtual void callVisitor(QstItemVisitor* visitor) = 0;
    QQmlListProperty<QObject> nested();
    virtual void handleParserEvent(ParserEvent event) = 0;
    QList<QObject*> m_nested;

private:
    void afterClassBegin();
    void afterComponentComplete();
    virtual void classBegin() final;
    virtual void componentComplete() final;

};

inline QQmlListProperty<QObject> QstItem::nested() { return QQmlListProperty<QObject>(this, m_nested); }


#endif // PARSERSTATEHANDLER_H
