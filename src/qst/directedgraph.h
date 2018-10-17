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
#ifndef Dag_H
#define Dag_H

#include <QtCore/QMultiMap>
#include <QtCore/QDebug>

template<typename NodeT, typename EdgeT = void*>
class DirectedGraph
{
public:
    using NodeId = uint;
    using EdgeId = QPair<NodeId, NodeId>;

    void dump();
    bool isEmpty() const;

    void insertEdge(const NodeT& predecessor, const NodeT& successor, const EdgeT& edge = EdgeT());
    void insertNode(const NodeT& node);

    EdgeT edge(const NodeT& predecessor, const NodeT& successor) const;
    QList<EdgeT> edges(const NodeT& predecessor, const NodeT& successor) const;
    QList<EdgeT> edges() const;
    QList<NodeT> nodes() const;
    QList<NodeT> roots() const;

    QList<NodeT> predecessors(const NodeT& node) const;
    QList<NodeT> successors(const NodeT& node) const;

private:
    QList<NodeT> nodes(const QList<NodeId>& ids) const;
    NodeId id(const NodeT& node) const;

    QMultiMap<NodeId, NodeId> m_successors;
    QMultiMap<NodeId, NodeId> m_predecessors;
    QMap<NodeId, NodeT> m_nodes;
    QMultiMap<EdgeId, EdgeT> m_edges;
};

template <typename NodeT, typename EdgeT>
void DirectedGraph<NodeT, EdgeT>::insertEdge(const NodeT& parent, const NodeT& child, const EdgeT& edge)
{
    NodeId parentId = id(parent);
    NodeId childId = id(child);
    m_successors.insert(parentId, childId);
    m_predecessors.insert(childId, parentId);
    m_edges.insert(EdgeId(parentId, childId), edge);
}

template <typename NodeT, typename EdgeT>
void DirectedGraph<NodeT, EdgeT>::insertNode(const NodeT& node)
{
    m_nodes.insert(id(node), node);
}

template <typename NodeT, typename EdgeT>
bool DirectedGraph<NodeT, EdgeT>::isEmpty() const
{
    return m_nodes.isEmpty();
}

template <typename NodeT, typename EdgeT>
EdgeT DirectedGraph<NodeT, EdgeT>::edge(const NodeT& predecessor, const NodeT& successor) const
{
    return m_edges.value(EdgeId(id(predecessor), id(successor)));
}

template <typename NodeT, typename EdgeT>
QList<EdgeT> DirectedGraph<NodeT, EdgeT>::edges(const NodeT& predecessor, const NodeT& successor) const
{
    return m_edges.values(EdgeId(id(predecessor), id(successor)));
}

template <typename NodeT, typename EdgeT>
QList<EdgeT> DirectedGraph<NodeT, EdgeT>::edges() const
{
    return m_edges.values();
}

template <typename NodeT, typename EdgeT>
QList<NodeT> DirectedGraph<NodeT, EdgeT>::nodes() const
{
    return m_nodes.values();
}

template <typename NodeT, typename EdgeT>
QList<NodeT> DirectedGraph<NodeT, EdgeT>::nodes(const QList<NodeId>& ids) const
{
    QList<NodeT> result;
    for (const auto& id: ids)
    {
        result << m_nodes.value(id);
    }
    return result;
}


template <typename NodeT, typename EdgeT>
QList<NodeT> DirectedGraph<NodeT, EdgeT>::predecessors(const NodeT& node) const
{
    return nodes(m_predecessors.values(id(node)));
}

template <typename NodeT, typename EdgeT>
QList<NodeT> DirectedGraph<NodeT, EdgeT>::successors(const NodeT& node) const
{
    return nodes(m_successors.values(id(node)));
}

template <typename NodeT, typename EdgeT>
QList<NodeT> DirectedGraph<NodeT, EdgeT>::roots() const
{
    QList<NodeT> result;
    for (const auto& node: m_nodes)
    {
        if (!m_predecessors.contains(id(node)))
        {
            result << node;
        }
    }
    return result;
}

template <typename NodeT, typename EdgeT>
void DirectedGraph<NodeT, EdgeT>::dump()
{
    QStringList nodes;
    for (const auto& node: m_nodes)
    {
        nodes << QString("%1=%2").arg(id(node)).arg(node.testcase()->name());
    }
    qDebug() << "Nodes: " << nodes;
    qDebug() << "Predecessors: " << m_predecessors;
    qDebug() << "Successors: " << m_successors;
}

template <typename NodeT, typename EdgeT>
uint DirectedGraph<NodeT, EdgeT>::id(const NodeT& node) const
{
    return qHash(node);
}

#endif // DIRECTEDGRAPH_H
