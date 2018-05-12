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
#include "applicationoptions.h"
#include "hierarchyvalidator.h"
#include "qstitem.h"
#include "project.h"
#include "projectresolver.h"
#include "qst.h"
#include "textfile.h"

#include <QtCore/QCoreApplication>
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

ProjectResolver::ProjectResolver(const QVariantMap& profile)
{
    m_currentDocument = nullptr;
    m_profile = profile;
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
    // 1. Create a root object with all const properties.
    QSharedPointer<QstDocument> rootDocument = this->beginCreate(rootfilepath);
    if (hasErrors())
    {
        return;
    }

    // One test case in a single file. Create a dummy project item in that case.
    if (rootDocument->object->baseTypeInfo() == &Testcase::staticMetaObject)
    {
        Testcase* testCase = qobject_cast<Testcase*>(rootDocument->object);
        // Stand-alone test case. We need a default Project component
        // to satisfy minimum requirements.
        QSharedPointer<QstDocument> projectDoc = this->createDefaultProjectComponent();
        m_project = qobject_cast<Project*>(projectDoc->object);
        m_documents.append(projectDoc);
        rootDocument->context->setContextProperty("test", testCase);
    }
    // A project with (multiple) testcases and file references.
    else if (rootDocument->object->baseTypeInfo() == &Project::staticMetaObject)
    {
        m_project = qobject_cast<Project*>(rootDocument->object);
    }
    else
    {
        // beginCreate should have done that for us
        Q_ASSERT(false);
    }

    rootDocument->context->setContextProperty("project", m_project.data());
    this->completeCreate(rootDocument);
    if (this->hasErrors())
    {
        return;
    }
    m_documents.append(rootDocument);

    // 2. Browse and resolve references in project items.
    // Project items might be recursive.
    QStringList absoluteReferences = makeAbsolute(m_project->references(),
            rootDocument->context->contextProperty("path").toString());

    QSet<QString> resolved{ QFileInfo(rootfilepath).absoluteFilePath() };
    for (QStringList unresolved = absoluteReferences; !unresolved.isEmpty(); unresolved.removeFirst())
    {
        const QString& reference = unresolved.first();
        if (resolved.contains(reference))
        {
            m_errors.append(QString("While resolving Project references: Document %1 is referenced multiple times.")
                            .arg(reference));
            return;
        }
        unresolved << resolveProjectReference(reference);
        resolved.insert(reference);
    }

    // 3. Find duplicate testcases
    QMap<QString, Testcase*> testcases;
    for (const auto& document: m_documents)
    {
        QList<Testcase*> children = document->object->findChildren<Testcase*>();
        if (document->object->baseTypeInfo() == &Testcase::staticMetaObject)
        {
            children << qobject_cast<Testcase*>(document->object);
        }
        for (const auto& testcase: children)
        {
            if (!testcases.contains(testcase->name()))
            {
                testcases.insert(testcase->name(), testcase);
            }
            else
            {
                QmlContext existing = qst::qmlDefinitionContext(testcases.value(testcase->name()));
                QmlContext duplicate = qst::qmlDefinitionContext(testcase);
                m_errors.append(QString("At %1:%2: A testcase with that name was already defined at %3:%4.")
                                .arg(duplicate.file()).arg(duplicate.line())
                                .arg(existing.file()).arg(existing.line()));
            }
        }
    }
}

QStringList ProjectResolver::resolveProjectReference(const QString& filepath)
{
    QStringList unresolvedFiles;

    if (!QFile::exists(filepath))
    {
        m_errors.append(QString("File '%1' referenced by project '%2' does not exist.")
                        .arg(filepath).arg(m_currentDocument.toStrongRef()->factory->url().path()));

        return QStringList();
    }
    QSharedPointer<QstDocument> document = beginCreate(filepath);
    if (document->state == QstDocument::Invalid)
    {
        return QStringList();
    }

    if (document->object->baseTypeInfo() == &Project::staticMetaObject)
    {
        QStringList references = qobject_cast<Project*>(document->object)->references();
        unresolvedFiles.append(makeAbsolute(references,
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
        return QStringList();
    }

    QString name = document->object->property("name").toString();
    m_documents.append(document);

    return unresolvedFiles;
}

// Begins component creation, performs various sanity checks and returns
// everything bundled in an Item container.
QSharedPointer<QstDocument> ProjectResolver::beginCreate(const QString& filepath)
{
    QSharedPointer<QstDocument> document = QSharedPointer<QstDocument>(new QstDocument());
    m_currentDocument = document.toWeakRef();
    document->engine = createEngine();

    // Re-route QML error handling only in the scope of this method.
    // Cope with multiple exit points.
    QmlEngineWarningScopeGuard guard(this, document->engine.data());

    document->state = QstDocument::Undefined;
    document->context = new QQmlContext(document->engine->rootContext(), this);
    document->context->setContextProperty("profile", m_profile);
    document->context->setContextProperty("path", QFileInfo(filepath).dir().absolutePath());
    document->factory = new QQmlComponent(document->engine, filepath, this);

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


void ProjectResolver::completeCreate(const QSharedPointer<QstDocument>& document)
{
    // Re-route QML error handling only in the scope of this method.
    // Cope with multiple exit points.
    QmlEngineWarningScopeGuard guard(this, document->engine.data());

    Q_ASSERT(document->state == QstDocument::Creating);

    QSharedPointer<QstDocument> currentItemBackup = m_currentDocument;
    m_currentDocument = document;

    document->factory->completeCreate();
    if (document->factory->isError()) \
    {
        for (const auto& error : document->factory->errors())
        {
            m_errors.append(error.toString());
        }
    }

    QList<QstItem*> children = document->object->findChildren<QstItem*>() << document->object;
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

    project->engine = createEngine();
    project->factory = new QQmlComponent(project->engine, this);
    project->factory->setData("import qst 1.0\n Project {}", QUrl());
    project->object = qobject_cast<QstItem*>(project->factory->beginCreate(project->engine->rootContext()));
    qobject_cast<Project*>(project->object)->afterClassBegin();
    project->factory->completeCreate();
    qobject_cast<Project*>(project->object)->afterComponentComplete();
    project->state = QstDocument::Finished;

    Q_ASSERT(!project->factory->isError());
    Q_ASSERT(!project->object.isNull());

    m_currentDocument = currentDocumentBackup;
    return project;
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

QQmlEngine* ProjectResolver::createEngine()
{
    QQmlEngine* engine = new QQmlEngine(this);
    QObject::connect(engine, &QQmlEngine::quit, QCoreApplication::instance(), &QCoreApplication::quit);

    for (const auto& path : ApplicationOptions::instance()->importPaths)
    {
        engine->addImportPath(path);
    }

    TextFile::registerJSType(engine);

    return engine;
}
