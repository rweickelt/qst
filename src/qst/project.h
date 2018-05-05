/****************************************************************************
 **
 ** Copyright (C) 2017-2018 The Qst project.
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
#ifndef PROJECT_H
#define PROJECT_H

#include "qstitem.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtQml/QQmlListProperty>

class Project : public QstItem
{
    Q_OBJECT

    friend class ProjectResolver;

public:
    explicit Project(QObject *parent = 0);

    Q_PROPERTY(QString name READ objectName WRITE setObjectName NOTIFY nameChanged)
    Q_PROPERTY(QStringList references READ references WRITE setReferences)
    Q_PROPERTY(QString workingDirectory READ workingDirectory CONSTANT)

    virtual const QMetaObject* baseTypeInfo() const final;
    QString name() const;
    QStringList references() const;
    void setReferences(const QStringList& references);
    QString workingDirectory() const;

signals:
    void nameChanged(const QString&);

protected:
    virtual void callVisitor(QstItemVisitor* visitor) final;
    virtual void handleParserEvent(QstItem::ParserEvent event);

private:
    QStringList m_references;
    QString m_workingDirectory;
};

inline const QMetaObject* Project::baseTypeInfo() const { return &Project::staticMetaObject; }

inline QString Project::name() const { return objectName(); }

inline QStringList Project::references() const { return m_references; }

inline QString Project::workingDirectory() const { return m_workingDirectory; }

inline void Project::setReferences(const QStringList& references)
{
    m_references = references;
}



#endif // PROJECT_H
