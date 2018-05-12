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
#include "testcase.h"

#include <QtDebug>


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
