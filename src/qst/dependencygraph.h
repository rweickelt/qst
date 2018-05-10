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

#ifndef DEPENDENCYGRAPH_H
#define DEPENDENCYGRAPH_H

#include <QtCore/QMap>
#include <QSharedData>
#include <QSharedDataPointer>
#include <QtCore/QString>

struct DependencyNode;
class Exports;
class QstDocument;
class Testcase;

struct DependencyNodeData : public QSharedData
{
    QStringList dependencies;
    QStringList targetNames;
    Exports* exportItem = nullptr;
    Testcase* testcaseItem = nullptr;
};

class DependencyNode
{
public:
    DependencyNode() : d(new DependencyNodeData()) {}
    DependencyNodeData* operator->() { return d.data(); }
    const DependencyNodeData* operator->() const { return d.data(); }

private:
    QSharedDataPointer<DependencyNodeData> d;
};

class DependencyGraph
{
    friend class DependencyResolver;
    friend class DependencyVisitor;
    using NodeId = QString;

public:
    DependencyGraph();

    void dump();
    QList<NodeId> children(const NodeId& id = NodeId()) const;
    bool isEmpty() const;
    DependencyNode& node(const NodeId& id);
    QList<DependencyNode> nodes() const;

private:
    QMultiMap<QString, QString> m_vertices;
    QMap<QString, DependencyNode> m_nodes;
};

class DependencyResolver
{
public:
    DependencyResolver(const QList<QstDocument*>& documents);
    DependencyGraph dependencies();

private:
    DependencyGraph m_graph;
};

inline DependencyGraph DependencyResolver::dependencies() { return m_graph; }

#endif // DEPENDENCYGRAPH_H
