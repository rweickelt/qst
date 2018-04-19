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
#include "matrix.h"
#include "parsereventhandler.h"
#include "project.h"
#include "projectresolver.h"
#include "qst.h"

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

ProjectResolver::ProjectResolver(QQmlEngine* engine)
{
    m_currentDocument = nullptr;
    m_engine = engine;
    projectResolver = this;
}


void ProjectResolver::loadRootFile(const QString& rootfilepath)
{
    // Re-route QML error handling only in the scope of this method.
    // Cope with multiple exit points.
    QmlEngineWarningScopeGuard guard(this, m_engine.data());

    // 1. Create a root object with all const properties.
    Document rootItem = this->beginCreate(rootfilepath);
    if (rootItem.state == Document::Invalid)
    {
        return;
    }

    // One test case in a single file.
    // Create a dummy project item in that case.
    if (rootItem.qstBaseType == "Testcase")
    {
        Testcase* testCase = qobject_cast<Testcase*>(rootItem.rootObject);
        // Stand-alone test case. We need a default Project component
        // to satisfy minimum requirements.
        Document projectComponent = this->createDefaultProjectComponent();
        m_project = qobject_cast<Project*>(projectComponent.rootObject);
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
        m_project = qobject_cast<Project*>(rootItem.rootObject);
        // In-line test cases need project context property
        rootItem.context->setContextProperty("project", m_project);

        for (const auto& child : rootItem.components)
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
        m_errors.append(QString("%1 is not allowed as root component in %2. Allowed types are Project, Testcase.")
                        .arg(rootItem.rootObject->metaObject()->className()).arg(rootfilepath));
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
        Document item = beginCreate(unresolved.first());
        if (item.state == Document::Invalid)
        {
            return;
        }

        if (item.qstBaseType == "Project")
        {
            QStringList references = qobject_cast<Project*>(item.rootObject)->references();
            unresolved.append(makeAbsolute(references,
                    item.context->contextProperty("path").toString()));

            for (const auto& child : item.components)
            {
                if (child->typeName() == "Testcase")
                {
                    m_testCases.append(qobject_cast<Testcase*>(child));
                }
            }
        }
        else if (item.qstBaseType == "Testcase")
        {
            m_testCases.append(qobject_cast<Testcase*>(item.rootObject));
            item.context->setContextProperty("project", m_project);
            item.context->setContextProperty("test", item.rootObject);
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

        QString name = item.rootObject->property("name").toString();
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
ProjectResolver::Document ProjectResolver::beginCreate(const QString& filepath)
{
    Document item;
    m_currentDocument = &item;
    item.state = Document::Undefined;
    item.filepath  = filepath;
    item.context = new QQmlContext(m_engine->rootContext(), this);
    item.context->setContextProperty("path", QFileInfo(filepath).dir().absolutePath());
    item.qmlComponent = new QQmlComponent(m_engine, filepath, this);
    item.rootObject = item.qmlComponent->beginCreate(item.context);

    if (item.qmlComponent->isError()) \
    {
        item.state = Document::Invalid;
        for (const auto& error : item.qmlComponent->errors())
        {
            m_errors.append(error.toString());
        }
        return item;
    }

    Q_ASSERT(!item.rootObject.isNull());

    for (const auto& child: item.handlers)
    {
        child->afterClassBegin();
    }

    // 1. Cache the relevant base type information instead of always looking at meta object.
    // 2. Perform validity checks
    if (item.rootObject->metaObject()->inherits(&Project::staticMetaObject))
    {
        item.qstBaseType = "Project";
    }
    else if (item.rootObject->metaObject()->inherits(&Testcase::staticMetaObject))
    {
        item.qstBaseType = "Testcase";
        Testcase* test = qobject_cast<Testcase*>(item.rootObject);
        for (const auto& child: item.components)
        {
            test->registerChild(child);
        }
    }
    else if (item.rootObject->metaObject()->inherits(&Matrix::staticMetaObject))
    {
        item.qstBaseType = "Matrix";
    }
    else
    {
        m_errors.append(QString("The type %1 is not allowed as document root in %2. Allowed types are Matrix, Project, Testcase")
                        .arg(item.rootObject->metaObject()->className())
                        .arg(filepath));
        item.state = Document::Invalid;
        return item;
    }

    item.state = Document::Creating;
    return item;
}


void ProjectResolver::completeCreate(Document* item)
{
    Q_ASSERT(item != nullptr);
    Q_ASSERT(item->state == Document::Creating);

    Document* currentItemBackup = m_currentDocument;
    m_currentDocument = item;

    item->qmlComponent->completeCreate();
    if (item->qmlComponent->isError()) \
    {
        for (const auto& error : item->qmlComponent->errors())
        {
            m_errors.append(error.toString());
        }
    }

    for (const auto& child: item->handlers)
    {
        child->afterComponentComplete();
    }

    if (item->qstBaseType == "Testcase")
    {
        Testcase* test = static_cast<Testcase*>(item->rootObject.data());
        if (test->hasErrors())
        {
            m_errors.append(test->errorString());
        }
    }

    m_currentDocument = currentItemBackup;
}

ProjectResolver::Document ProjectResolver::createDefaultProjectComponent()
{
    Document* currentItemBackup = m_currentDocument;

    Document project;
    m_currentDocument = &project;

    project.qmlComponent = new QQmlComponent(m_engine, this);
    project.qmlComponent->setData("import qst 1.0\n Project {}", QUrl());
    project.rootObject = project.qmlComponent->beginCreate(m_engine->rootContext());
    qobject_cast<Project*>(project.rootObject)->afterClassBegin();
    project.qmlComponent->completeCreate();
    qobject_cast<Project*>(project.rootObject)->afterComponentComplete();
    project.state = Document::Finished;

    Q_ASSERT(!project.qmlComponent->isError());
    Q_ASSERT(!project.rootObject.isNull());

    m_currentDocument = currentItemBackup;
    return project;
}

QList<Testcase*> ProjectResolver::testcases() const
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

ProjectResolver::Document* ProjectResolver::currentDocument()
{
    Q_ASSERT(m_currentDocument != nullptr);
    return m_currentDocument;
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

void ProjectResolver::registerMatrixComponent(Matrix* matrix)
{
    m_matrices << matrix;
}
