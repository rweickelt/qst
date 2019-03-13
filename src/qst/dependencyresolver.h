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

#ifndef DEPENDENCYRESOLVER_H
#define DEPENDENCYRESOLVER_H

#include "dependency.h"
#include "directedgraph.h"
#include "job.h"
#include "resource.h"

#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QStringList>

class Exports;
class QstDocument;
class ResourceItem;

class DependencyResolver
{
public:
    DependencyResolver();
    void beginResolve(const QList<QstDocument*>& documents);
    void completeResolve(const JobTable& jobs, const ResourceTable& resources);
    DirectedGraph<Job, Dependency> jobGraph();

    QStringList errors() const { return m_errors; }
    bool hasErrors() const { return !m_errors.isEmpty(); }

private:
    friend class DependencyVisitor;
    friend class ItemGatherVisitor;

    QMultiMap<QString, ResourceItem*> m_resources;
    QMap<QString, Testcase*> m_testcases;
    QHash<QString, Exports*> m_exports;

    DirectedGraph<QString, Depends*> m_resourceGraph;
    DirectedGraph<QString, Depends*> m_testcaseGraph;

    DirectedGraph<Job, Dependency> m_jobGraph;
    QMultiMap<Job, QPair<Dependency, Resource> > m_resourcesPerJob;
    QStringList m_errors;
};

inline DirectedGraph<Job, Dependency> DependencyResolver::jobGraph() { return m_jobGraph; }


#endif // DEPENDENCYRESOLVER_H
