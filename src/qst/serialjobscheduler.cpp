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

#include "depends.h"
#include "exports.h"
#include "serialjobscheduler.h"
#include "qstdocument.h"
#include "qstitemvisitor.h"
#include "testcase.h"

#include <QtQml/QQmlContext>
#include <QtDebug>

namespace {
//    QStringList readylistToString(QList<Job> jobs)
//    {
//        QStringList names;
//        for (const auto& job: jobs)
//        {
//            names << job.testcase()->name();
//        }
//        return names;
//    }
}


SerialJobScheduler::SerialJobScheduler(const DirectedGraph<Job, Dependency>& jobs, QObject* parent)
    : QObject(parent)
{
    m_dependencies = jobs;
}

void SerialJobScheduler::onJobFinished(Job finishedJob)
{
    QString name = finishedJob.testcase()->name();
    m_done.append(finishedJob);

    if (Exports* exports = finishedJob.testcase()->exportsItem())
    {
        finishedJob.setExports(parseExports(exports));
    }

    for (const auto& dependent: m_dependencies.successors(finishedJob))
    {
        m_dependencyCounts[dependent] -= 1;
        if (m_dependencyCounts[dependent] == 0)
        {
            m_dependencyCounts.remove(dependent);
            m_readyList << m_todo.takeAt(m_todo.indexOf(dependent));
        }
    }

    if (!m_readyList.isEmpty())
    {
        Job nextJob = m_readyList.takeFirst();

        QMultiMap<QString, QVariant> multiDependencies;

        for (const auto& predecessor: m_dependencies.predecessors(nextJob))
        {
            if (!predecessor.testcase()->exportsItem())
            {
                continue;
            }

            Dependency dependency = m_dependencies.edge(predecessor, nextJob);
            QString alias = dependency.alias().isEmpty() ? dependency.name() : dependency.alias();
            multiDependencies.insert(alias, predecessor.exports());
        }

        for (const auto& alias: multiDependencies.keys())
        {
            nextJob.testcase()->attachDependencyExport(alias, multiDependencies.values(alias));
        }

        emit jobReady(nextJob);
        return;
    }
    else
    {
        emit finished();
    }
}

void SerialJobScheduler::start()
{
    if (m_dependencies.isEmpty())
    {
        emit finished();
        return;
    }

    Q_ASSERT(!m_dependencies.roots().isEmpty());

    m_done.clear();
    m_todo = m_dependencies.nodes();

    for (const auto& job: m_dependencies.roots())
    {
        m_readyList << m_todo.takeAt(m_todo.indexOf(job));
    }

    for (const auto& job: m_dependencies.nodes())
    {
        m_dependencyCounts.insert(job, m_dependencies.predecessors(job).length());
    }

    Q_ASSERT(!m_readyList.isEmpty());

    emit jobReady(m_readyList.takeFirst());
}


QVariantMap SerialJobScheduler::parseExports(Exports* item)
{
    const static QStringList ignoredProperties{ "objectName", "nestedComponents" };
    QVariantMap result;
    const QMetaObject *metaobject = item->metaObject();
    int count = metaobject->propertyCount();
    for (int i=0; i<count; ++i) {
        QMetaProperty metaproperty = metaobject->property(i);
        const char *name = metaproperty.name();

        if (ignoredProperties.contains(QLatin1String(name)) || (!metaproperty.isReadable()))
        {
            continue;
        }

        QVariant value = item->property(name);
        result[QLatin1String(name)] = value;
    }
    return result;
}

