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

#include "dependencygraph.h"
#include "depends.h"
#include "qstdocument.h"
#include "qstitemvisitor.h"
#include "testcase.h"

#include <QtCore/QVariantMap>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtDebug>

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
    QQmlContext* context = qmlContext((*m_currentNode)->testcaseItem)->parentContext();
    Q_ASSERT(context);
    context->setContextProperty(name.toLatin1().constData(), QVariantMap());
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

DependencyGraph::DependencyGraph()
{

}

QList<DependencyGraph::NodeId> DependencyGraph::children(const DependencyGraph::NodeId& item) const
{
    return m_vertices.values(item);
}

bool DependencyGraph::isEmpty() const
{
    return m_nodes.isEmpty();
}


DependencyNode& DependencyGraph::node(const NodeId& id)
{
    Q_ASSERT(m_nodes.contains(id));
    return m_nodes[id];
}

QList<DependencyNode> DependencyGraph::nodes() const
{
    return m_nodes.values();
}


void DependencyGraph::dump()
{
    QStringList nodes;
    for (const auto& node: m_nodes)
    {
        nodes << node->testcaseItem->name();
    }
    qDebug() << "Nodes: " << nodes;
    qDebug() << "Vertices: " << m_vertices;
}

DependencyResolver::DependencyResolver(const QList<QstDocument*> &documents)
{
    if (documents.isEmpty())
    {
        return;
    }

    DependencyVisitor visitor(&m_graph);
    for (auto& document: documents)
    {
        document->object->accept(&visitor);
    }

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
}
