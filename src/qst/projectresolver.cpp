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
#include "console.h"
#include "logger.h"
#include "projectresolver.h"
#include "qst.h"
#include "parsereventhandler.h"
#include "project.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTimer>

#include <QtQml/QQmlComponent>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>

#include <QDebug>

class QmlEngineWarningScopeGuard
{
public:
    inline QmlEngineWarningScopeGuard(ProjectResolver* resolver, QQmlEngine* engine) {
        m_resolver = resolver;
        m_engine = engine;
        m_engine->connect(m_engine, &QQmlEngine::warnings, m_resolver, &ProjectResolver::onQmlEngineWarnings);
        m_engine->setOutputWarningsToStandardError(false);
    }

    inline ~QmlEngineWarningScopeGuard()
    {
        m_engine->setOutputWarningsToStandardError(true);
        m_engine->disconnect(m_engine, &QQmlEngine::warnings, m_resolver, &ProjectResolver::onQmlEngineWarnings);
    }

private:
    ProjectResolver* m_resolver;
    QQmlEngine* m_engine;
};

namespace {
    ProjectResolver* projectResolver = nullptr;
}

ProjectResolver::ProjectResolver(QQmlEngine* engine, const QString& rootfilepath)
{
    m_currentItem = nullptr;
    m_engine = engine;
    m_rootFilepath = rootfilepath;
    projectResolver = this;
}


void ProjectResolver::loadRootFile()
{
    // Re-route QML error handling only in the scope of this method.
    // Cope with multiple exit points.
    QmlEngineWarningScopeGuard guard(this, m_engine.data());

    // 1. Create a root object with all const properties.
    Item rootItem = this->beginCreate(m_rootFilepath);
    if (rootItem.state == Item::Invalid)
    {
        return;
    }

    for (const auto& child: rootItem.children)
    {
        child->handleParserEvent(ParserEventHandler::AfterClassBegin);
    }

    // One test case in a single file.
    // Create a dummy project item in that case.
    if (rootItem.qstBaseType == "Testcase")
    {
        Testcase* testCase = qobject_cast<Testcase*>(rootItem.object);
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
    // Project item that may reference files with test cases,
    // but also contain test case items.
    else if (rootItem.qstBaseType == "Project")
    {
        m_project = qobject_cast<Project*>(rootItem.object);
        m_project->handleParserEvent(ParserEventHandler::AfterClassBegin);
        // In-line test cases need project context property
        rootItem.context->setContextProperty("project", m_project);

        for (const auto& child : rootItem.children)
        {
            if (child->typeName() == "Testcase")
            {
                m_testCases.append(qobject_cast<Testcase*>(child));
            }
        }
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

    // 2. Browse and resolve references in root project item.
    QStringList absoluteReferences = makeAbsolute(m_project->references(),
            rootItem.context->contextProperty("path").toString());
    for (QStringList unresolved = absoluteReferences; !unresolved.isEmpty();
         unresolved.removeFirst())
    {
        if (!QFile::exists(unresolved.first()))
        {
            m_errors.append(QString("File '%1' referenced by project '%2' does not exist.")
                            .arg(unresolved.first()).arg(rootItem.filepath));
            return;
        }
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

            for (const auto& child : item.children)
            {
                if (child->typeName() == "Testcase")
                {
                    m_testCases.append(qobject_cast<Testcase*>(child));
                }
            }
        }
        else if (item.qstBaseType == "Testcase")
        {
            m_testCases.append(qobject_cast<Testcase*>(item.object));
            item.context->setContextProperty("project", m_project);
            item.context->setContextProperty("test", item.object);
        }

        // TODO: I's not possible to create more than 9 objects
        //       partially. QTBUG-47633. We need to finish creation
        //       here. Thus, resolving dependencies before complete
        //       creation is impossible.
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

// Begins component creation, performs various sanity checks and returns
// everything bundled in an Item container.
ProjectResolver::Item ProjectResolver::beginCreate(const QString& filepath)
{
    Item item;
    m_currentItem = &item;
    item.state = Item::Undefined;
    item.filepath  = filepath;
    item.context = new QQmlContext(m_engine->rootContext(), this);
    item.context->setContextProperty("path", QFileInfo(filepath).dir().absolutePath());
    item.factory = new QQmlComponent(m_engine, filepath, this);
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

    for (const auto& child: item.children)
    {
        child->handleParserEvent(ParserEventHandler::AfterClassBegin);
    }

    // 1. Cache the relevant base type information instead of always looking at meta object.
    // 2. Perform validity checks
    if (item.object->metaObject()->inherits(&Project::staticMetaObject))
    {
        item.qstBaseType = "Project";
    }
    else if (item.object->metaObject()->inherits(&Testcase::staticMetaObject))
    {
        item.qstBaseType = "Testcase";
        Testcase* test = qobject_cast<Testcase*>(item.object);
        for (const auto& child: item.children)
        {
            test->registerChild(child);
        }
    }
    else
    {
        m_errors.append(QString("The type %1 is not allowed as root component in %2. Allowed types are Project, Testcase")
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
    Q_ASSERT(item != nullptr);
    Q_ASSERT(item->state == Item::Creating);

    m_currentItem = item;
    item->factory->completeCreate();
    if (item->factory->isError()) \
    {
        for (const auto& error : item->factory->errors())
        {
            m_errors.append(error.toString());
        }
    }

    for (const auto& child: item->children)
    {
        child->handleParserEvent(ParserEventHandler::AfterComponentComplete);
    }

    if (item->qstBaseType == "Testcase")
    {
        Testcase* test = static_cast<Testcase*>(item->object.data());
        if (test->hasErrors())
        {
            m_errors.append(test->errorString());
        }
    }
}

ProjectResolver::Item ProjectResolver::createDefaultProjectComponent()
{
    Item project;

    project.factory = new QQmlComponent(m_engine, this);
    project.factory->setData("import qst 1.0\n Project {}", QUrl());
    project.object = project.factory->beginCreate(m_engine->rootContext());
    qobject_cast<Project*>(project.object)->handleParserEvent(ParserEventHandler::AfterClassBegin);
    project.factory->completeCreate();
    project.state = Item::Finished;

    Q_ASSERT(!project.factory->isError());
    Q_ASSERT(!project.object.isNull());


    return project;
}

QList<QPointer<Testcase> > ProjectResolver::testcases() const
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
        else
        {
            sanitized << QDir::cleanPath(filepath);
        }
    }
    return sanitized;
}

/*
This slot is invoked whenever an exception in the QML engine observes
an exception.
*/
void ProjectResolver::onQmlEngineWarnings(const QList<QQmlError> &warnings)
{
    Q_ASSERT(warnings.size() == 1);
    const QQmlError& error = warnings.first();

    // Exception caused by QML code. This includes syntax and reference errors.
    QString message = QString("In %1:%2 %3")
            .arg(error.url().toString())
            .arg(error.line())
            .arg(error.description());
    QST_ERROR_AND_EXIT(message);

}

ProjectResolver::Item* ProjectResolver::currentItem()
{
    Q_ASSERT(m_currentItem != nullptr);
    return m_currentItem;
}

ProjectResolver* ProjectResolver::instance()
{
    Q_ASSERT(projectResolver != nullptr);
    return projectResolver;
}

Project* ProjectResolver::project()
{
    return m_project.data();
}
