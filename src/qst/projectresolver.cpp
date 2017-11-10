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
#include "console.h"
#include "log.h"
#include "projectresolver.h"
#include "qst.h"
#include "project.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTimer>

#include <QtQml/QQmlComponent>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQml/private/qqmlcontext_p.h>

ProjectResolver::ProjectResolver(QQmlEngine* engine, const QString& rootfilepath)
{
    m_engine = engine;
    m_rootFilepath = rootfilepath;
}


void ProjectResolver::loadRootFile()
{
    // 1. Create a root object with all const properties.
    Item rootItem = this->beginCreate(m_rootFilepath);
    if (rootItem.state == Item::Invalid)
    {
        return;
    }

    if (rootItem.qstBaseType == "TestCase")
    {
        TestCase* testCase = qobject_cast<TestCase*>(rootItem.object);
        // Stand-alone test case. We need a default Project component
        // to satisfy minimum requirements.
        Item projectComponent = this->createDefaultProjectComponent();
        m_project = qobject_cast<Project*>(projectComponent.object);
        m_components.insert("project", projectComponent);
        m_components.insert(testCase->name(), projectComponent);
        m_testCases.append(testCase);
        rootItem.context->setContextProperty("test", testCase);
        rootItem.context->setContextProperty("project", m_project);
        this->completeCreate(&rootItem);
        if (this->hasErrors())
        {
            return;
        }
    }
    else if (rootItem.qstBaseType == "Project")
    {
        m_project = qobject_cast<Project*>(rootItem.object);
        m_components.insert(m_project->name(), rootItem);
        this->completeCreate(&rootItem);
        if (this->hasErrors())
        {
            return;
        }
    }
    else
    {
        m_errors.append(QString("%1 is not allowed as root component in %2. Allowed types are Project, TestCase.")
                        .arg(rootItem.object->metaObject()->className()).arg(m_rootFilepath));
        return;
    }

    // 2. Browse and resolve references.
    QStringList absoluteReferences = makeAbsolute(m_project->references(),
            rootItem.context->contextProperty("path").toString());
    for (QStringList unresolved = absoluteReferences; !unresolved.isEmpty();
         unresolved.removeFirst())
    {
        Item item = beginCreate(unresolved.first());
        if (item.state == Item::Invalid)
        {
            return;
        }

        if (item.qstBaseType == "Project")
        {
            QStringList references = qobject_cast<Project*>(item.object)->references();
            unresolved.append(makeAbsolute(references,
                    item.context->contextProperty("path").toString()));
        }
        else if (item.qstBaseType == "TestCase")
        {
            m_testCases.append(qobject_cast<TestCase*>(item.object));
            item.context->setContextProperty("project", m_project);
            item.context->setContextProperty("test", item.object);
        }

        // TODO: I's not possible to create more than 9 objects
        //       partially. QTBUG-47633. We need to finish creation
        //       here. Thus, resolving dependencies is impossible.
        this->completeCreate(&item);
        if (hasErrors())
        {
            return;
        }

        QString name = item.object->property("name").toString();
        if (m_components.contains(name))
        {
            m_errors.append(QString("A component with the name '%1' already exists.").arg(name));
            return;
        }
        m_components.insert(name, item);
    }
}

ProjectResolver::Item ProjectResolver::beginCreate(const QString& filepath)
{
    Item item;

    item.context = new QQmlContext(m_engine->rootContext(), this);
    item.factory = new QQmlComponent(m_engine, filepath, this);
    item.state = Item::Undefined;
    item.filepath  = filepath;

    item.context->setContextProperty("path", QFileInfo(filepath).dir().absolutePath());

    item.object = item.factory->beginCreate(item.context);

    if (item.factory->isError()) \
    {
        item.state = Item::Invalid;
        for (const auto& error : item.factory->errors())
        {
            m_errors.append(error.toString());
        }
        return item;
    }

    Q_ASSERT(!item.object.isNull());

    if (item.object->metaObject()->inherits(&Project::staticMetaObject))
    {
        item.qstBaseType = "Project";
    }
    else if (item.object->metaObject()->inherits(&TestCase::staticMetaObject))
    {
        item.qstBaseType = "TestCase";
        if (item.object->property("name").toString().isEmpty())
        {
            m_errors.append(QString("TestCase component in %1 must define a unique name.").arg(filepath));
            item.state = Item::Invalid;
            return item;
        }
    }
    else
    {
        m_errors.append(QString("The type %1 is not allowed as root component in %2. Allowed types are Project, TestCase")
                        .arg(item.object->metaObject()->className())
                        .arg(filepath));
        item.state = Item::Invalid;
        return item;
    }

    item.state = Item::Creating;
    return item;
}


void ProjectResolver::completeCreate(Item* item)
{
    item->factory->completeCreate();
    if (item->factory->isError()) \
    {
        for (const auto& error : item->factory->errors())
        {
            m_errors.append(error.toString());
        }
    }
}

ProjectResolver::Item ProjectResolver::createDefaultProjectComponent()
{
    Item project;

    project.factory = new QQmlComponent(m_engine, this);
    project.factory->setData("import qst 1.0\n Project {}", QUrl());
    project.object = project.factory->create(m_engine->rootContext());
    project.state = Item::Finished;

    Q_ASSERT(!project.factory->isError());
    Q_ASSERT(!project.object.isNull());


    return project;
}

QList<QPointer<TestCase> > ProjectResolver::testcases() const
{
    return m_testCases;
}

QStringList ProjectResolver::makeAbsolute(const QStringList& paths, const QString& basePath)
{
    QStringList sanitized;
    for (const QString& filepath : paths)
    {
        if (QFileInfo(filepath).isRelative())
        {
            sanitized << QDir(basePath).absoluteFilePath(filepath);
        }
    }
    return sanitized;
}
