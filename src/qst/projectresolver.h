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
#ifndef PROJECTRESOLVER_H
#define PROJECTRESOLVER_H

#include "qstdocument.h"
#include "testcase.h"

#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QSharedPointer>
#include <QtCore/QStringList>
#include <QtCore/QWeakPointer>

class Matrix;
class QQmlEngine;
class QQmlComponent;
class QQmlContext;

/*
Starting from a QML root file, ProjectResolver assembles the whole project tree.

The root file can be either a Project Item or a single TestCase item.
Project items are browsed through and references are resolved.
References might be sub-projects and test cases.
If the root file is a TestCase item, then a default Project item is created.

ProjectResolver does not execute Test Cases.
*/
class ProjectResolver : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ProjectResolver)

public:
    ProjectResolver(QQmlEngine* engine);

    void appendError(const QString& message);
    QList<QstDocument*> documents();
    void registerMatrixComponent(Matrix* matrix);
    QstDocument* currentDocument();
    QStringList errors() const;
    bool hasErrors() const;
    void loadRootFile(const QString& rootfilepath);
    QList<Matrix*> matrices() const;
    Project* project();
    QList<Testcase*> testcases() const;

    static ProjectResolver* instance();

private:
    QSharedPointer<QstDocument> beginCreate(const QString& filepath);
    void completeCreate(const QSharedPointer<QstDocument>& item);
    QSharedPointer<QstDocument> createDefaultProjectComponent();
    Q_INVOKABLE void onQmlEngineWarnings(const QList<QQmlError> &warnings);

    static QStringList makeAbsolute(const QStringList& paths, const QString& basePath);

    QList<QSharedPointer<QstDocument> > m_documents;
    QPointer<QQmlEngine> m_engine;
    QStringList m_errors;
    QPointer<Project> m_project;
    QMap<QString, Testcase*> m_testCases;
    QWeakPointer<QstDocument> m_currentDocument;

    friend class QmlEngineWarningScopeGuard;
};

inline QStringList ProjectResolver::errors() const { return m_errors; }
inline bool ProjectResolver::hasErrors() const { return !m_errors.isEmpty(); }

#endif // PROJECTRESOLVER_H
