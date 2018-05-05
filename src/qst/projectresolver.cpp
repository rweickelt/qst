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
#include "hierarchyvalidator.h"
#include "qstitem.h"
#include "project.h"
#include "projectresolver.h"
#include "qst.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>

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

void ProjectResolver::appendError(const QString& message)
{
    m_errors << message;
}

QList<QstDocument*> ProjectResolver::documents()
{
    QList<QstDocument*> result;
    for (const auto& document: m_documents)
    {
        result << document.data();
    }
    return result;
}


void ProjectResolver::loadRootFile(const QString& rootfilepath)
{
    // Re-route QML error handling only in the scope of this method.
    // Cope with multiple exit points.
    QmlEngineWarningScopeGuard guard(this, m_engine.data());

    // 1. Create a root object with all const properties.
    QSharedPointer<QstDocument> rootDocument = this->beginCreate(rootfilepath);
    if (hasErrors())
    {
        return;
    }

    // One test case in a single file.
    // Create a dummy project item in that case.
    if (rootDocument->object->baseTypeInfo() == &Testcase::staticMetaObject)
    {
        Testcase* testCase = qobject_cast<Testcase*>(rootDocument->object);
        // Stand-alone test case. We need a default Project component
        // to satisfy minimum requirements.
        QSharedPointer<QstDocument> projectDoc = this->createDefaultProjectComponent();
        m_project = qobject_cast<Project*>(projectDoc->object);
        m_documents.append(projectDoc);
        m_documents.append(rootDocument);
        rootDocument->context->setContextProperty("test", testCase);
        rootDocument->context->setContextProperty("project", m_project.data());
        this->completeCreate(rootDocument);
        if (this->hasErrors())
        {
            return;
        }
    }
    // Project item that may reference files with test cases,
    // but also contain test case items.
    else if (rootDocument->object->baseTypeInfo() == &Project::staticMetaObject)
    {
        m_project = qobject_cast<Project*>(rootDocument->object);
        // In-line test cases need project context property
        rootDocument->context->setContextProperty("project", m_project);
        m_documents.append(rootDocument);
        this->completeCreate(rootDocument);
        if (this->hasErrors())
        {
            return;
        }
    }

    // 2. Browse and resolve references in root project item.
    QStringList absoluteReferences = makeAbsolute(m_project->references(),
            rootDocument->context->contextProperty("path").toString());
    for (QStringList unresolved = absoluteReferences; !unresolved.isEmpty();
         unresolved.removeFirst())
    {
        if (!QFile::exists(unresolved.first()))
        {
            m_errors.append(QString("File '%1' referenced by project '%2' does not exist.")
                            .arg(unresolved.first()).arg(rootfilepath));
            return;
        }
        QSharedPointer<QstDocument> document = beginCreate(unresolved.first());
        if (document->state == QstDocument::Invalid)
        {
            return;
        }

        if (document->object->baseTypeInfo() == &Project::staticMetaObject)
        {
            QStringList references = qobject_cast<Project*>(document->object)->references();
            unresolved.append(makeAbsolute(references,
                    document->context->contextProperty("path").toString()));

        }
        else if (document->object->baseTypeInfo() == &Testcase::staticMetaObject)
        {
            document->context->setContextProperty("project", m_project);
            document->context->setContextProperty("test", document->object);
        }

        // TODO: I's not possible to create more than 9 objects
        //       partially. QTBUG-47633. We need to finish creation
        //       here. Thus, resolving dependencies before complete
        //       creation is impossible.
        this->completeCreate(document);
        if (hasErrors())
        {
            return;
        }

        QString name = document->object->property("name").toString();
        m_documents.append(document);
    }

    for (const auto& document: m_documents)
    {
        QList<Testcase*> testcases = document->object->findChildren<Testcase*>();
        if (document->object->baseTypeInfo() == &Testcase::staticMetaObject)
        {
            testcases << qobject_cast<Testcase*>(document->object.data());
        }

        for (const auto& testcase: testcases)
        {
            if (!m_testCases.contains(testcase->name()))
            {
                m_testCases.insert(testcase->name(), testcase);
            }
            else
            {
                QmlContext existing = qst::qmlDefinitionContext(m_testCases.value(testcase->name()));
                QmlContext duplicate = qst::qmlDefinitionContext(testcase);
                m_errors.append(QString("At %1:%2: A testcase with that name was already defined at %3:%4.")
                                .arg(duplicate.file()).arg(duplicate.line())
                                .arg(existing.file()).arg(existing.line()));
            }
        }
    }
}

// Begins component creation, performs various sanity checks and returns
// everything bundled in an Item container.
QSharedPointer<QstDocument> ProjectResolver::beginCreate(const QString& filepath)
{
    QSharedPointer<QstDocument> document = QSharedPointer<QstDocument>(new QstDocument());
    m_currentDocument = document.toWeakRef();
    document->state = QstDocument::Undefined;
    document->context = new QQmlContext(m_engine->rootContext(), this);
    document->context->setContextProperty("path", QFileInfo(filepath).dir().absolutePath());
    document->factory = new QQmlComponent(m_engine, filepath, this);

    QObject* object = document->factory->beginCreate(document->context);
    document->object = qobject_cast<QstItem*>(object);

    if (document->factory->isError()) \
    {
        document->state = QstDocument::Invalid;
        for (const auto& error : document->factory->errors())
        {
            m_errors.append(error.toString());
        }
        return document;
    }

    Q_ASSERT(!document->object.isNull());

    HierarchyValidator hierarchyValidator(*this);
    hierarchyValidator.validate(document->object);
    if (this->hasErrors())
    {
        return document;
    }

    QList<QstItem*> children = document->object->findChildren<QstItem*>() << document->object.data();
    for (const auto& child: children)
    {
        child->afterClassBegin();
    }

    document->state = QstDocument::Creating;
    return document;
}


void ProjectResolver::completeCreate(const QSharedPointer<QstDocument>& item)
{
    Q_ASSERT(item->state == QstDocument::Creating);

    QSharedPointer<QstDocument> currentItemBackup = m_currentDocument;
    m_currentDocument = item;

    item->factory->completeCreate();
    if (item->factory->isError()) \
    {
        for (const auto& error : item->factory->errors())
        {
            m_errors.append(error.toString());
        }
    }

    QList<QstItem*> children = item->object->findChildren<QstItem*>() << item->object;
    for (const auto& child: children)
    {
        child->afterComponentComplete();
    }

    m_currentDocument = currentItemBackup;
}

QSharedPointer<QstDocument> ProjectResolver::createDefaultProjectComponent()
{
    QSharedPointer<QstDocument> currentDocumentBackup = m_currentDocument;

    QSharedPointer<QstDocument> project(new QstDocument());
    m_currentDocument = project;

    project->factory = new QQmlComponent(m_engine, this);
    project->factory->setData("import qst 1.0\n Project {}", QUrl());
    project->object = qobject_cast<QstItem*>(project->factory->beginCreate(m_engine->rootContext()));
    qobject_cast<Project*>(project->object)->afterClassBegin();
    project->factory->completeCreate();
    qobject_cast<Project*>(project->object)->afterComponentComplete();
    project->state = QstDocument::Finished;

    Q_ASSERT(!project->factory->isError());
    Q_ASSERT(!project->object.isNull());

    m_currentDocument = currentDocumentBackup;
    return project;
}

QList<Testcase*> ProjectResolver::testcases() const
{
    return m_testCases.values();
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

QstDocument* ProjectResolver::currentDocument()
{
    Q_ASSERT(!m_currentDocument.isNull());
    return m_currentDocument.data();
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
