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

#include "dependencyresolver.h"
#include "depends.h"
#include "exports.h"
#include "job.h"
#include "qstdocument.h"
#include "qstitemvisitor.h"
#include "testcase.h"

#include <QtCore/QVariantMap>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>

class DependencyVisitor : public QstItemVisitor
{
    Q_DISABLE_COPY(DependencyVisitor)

public:
    DependencyVisitor(DependencyGraph* graph);

protected:
    virtual void visit(Depends* item) final;
    virtual void visit(Exports* item) final;
    virtual void visit(Testcase* item) final;

private:
    DependencyGraph* m_graph;
    QMap<QString, DependencyNode>::iterator m_currentNode;
};

DependencyVisitor::DependencyVisitor(DependencyGraph* graph)
{
    m_graph = graph;
}

void DependencyVisitor::visit(Depends* item)
{
    Q_ASSERT(!item->on().isEmpty());

    QString name = item->on();
    (*m_currentNode)->dependencies.append(name);

    // Set a dummy object to satisfy bindings referencing the dependency.
    // The object will be updated later by the result of "Exports".
//    QQmlContext* context = qmlContext((*m_currentNode)->testcaseItem)->parentContext();
//    Q_ASSERT(context);
//    context->setContextProperty(name.toLatin1().constData(), QVariantMap());
}

void DependencyVisitor::visit(Exports* item)
{
    Q_ASSERT((*m_currentNode)->exportItem == nullptr);
    (*m_currentNode)->exportItem = item;
}

void DependencyVisitor::visit(Testcase* item)
{
    m_currentNode = m_graph->m_nodes.insert(item->name(), DependencyNode());
    (*m_currentNode)->testcaseItem  = item;
}

DependencyResolver::DependencyResolver()
{

}

void DependencyResolver::beginResolve(const QList<QstDocument*> &documents)
{
//    if (documents.isEmpty())
//    {
//        return;
//    }

    Q_ASSERT(!documents.isEmpty());

    DependencyVisitor visitor(&m_graph);
    for (auto& document: documents)
    {
        document->object->accept(&visitor);
    }

    // Build dependency structure Testcase -> Dependent testaces
    for (auto& node: m_graph.m_nodes.values())
    {
        QString dependentName = node->testcaseItem->name();
        if (!node->dependencies.isEmpty())
        {
            for (const auto& name: node->dependencies)
            {
                m_graph.m_vertices.insert(name, dependentName);
            }
        }
        else
        {
            m_graph.m_vertices.insert(QString(), dependentName);
        }
    }

    // Attach preliminary exports item to dependents
    for (auto& node: m_graph.nodes())
    {
        if (node->exportItem != nullptr)
        {
            QVariantMap exportMap = node->exportItem->toVariantMap();
            QList<DependencyNode> dependents;
            for (const auto& dependentName: m_graph.m_vertices.values(node->testcaseItem->name()))
            {
                dependents << m_graph.m_nodes.values(dependentName);
            }
            for (auto& dependent: dependents)
            {
                int index = dependent->dependencies.indexOf(node->testcaseItem->name());
                // Is not implement yet.
//                QString targetName = dependent->targetNames.at(index);
                QString targetName = node->testcaseItem->name();
                QQmlContext* context = qmlContext(dependent->testcaseItem)->parentContext();
                context->setContextProperty(targetName.toLatin1(), exportMap);
            }
        }
    }



}

QList<Job*> DependencyResolver::rootJobs() const
{
    return QList<Job*>();
}

