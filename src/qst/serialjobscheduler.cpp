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
#include "projectdatabase.h"
#include "serialjobscheduler.h"
#include "qstdocument.h"
#include "qstitemvisitor.h"
#include "testcase.h"

SerialJobScheduler::SerialJobScheduler(ProjectDatabase* db, QObject* parent)
    : QObject(parent), m_db(db)
{
}

void SerialJobScheduler::onJobFinished(Job finishedJob)
{
    QString name = finishedJob.name();
    m_done.append(finishedJob);
    m_results.append(finishedJob.result());

    for (const auto& dependent: m_db->jobGraph.successors(finishedJob))
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

        QMap<QString, QVariantList> multiDependencies;

        for (const auto& predecessor: m_db->jobGraph.predecessors(nextJob))
        {
            if (predecessor.exports().isEmpty())
            {
                continue;
            }

            Dependency dependency = m_db->jobGraph.edge(predecessor, nextJob);
            QString alias = dependency.alias().isEmpty() ? dependency.name() : dependency.alias();
            multiDependencies[alias].append(predecessor.exports());
        }

        nextJob.setDependenciesData(multiDependencies);

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
    if (m_db->jobGraph.isEmpty())
    {
        emit finished();
        return;
    }

    Q_ASSERT(!m_db->jobGraph.roots().isEmpty());

    m_done.clear();
    m_todo = m_db->jobGraph.nodes();

    for (const auto& job: m_db->jobGraph.roots())
    {
        m_readyList << m_todo.takeAt(m_todo.indexOf(job));
    }

    for (const auto& job: m_db->jobGraph.nodes())
    {
        m_dependencyCounts.insert(job, m_db->jobGraph.predecessors(job).length());
    }

    Q_ASSERT(!m_readyList.isEmpty());

    emit jobReady(m_readyList.takeFirst());
}

QList<Testcase::Result> SerialJobScheduler::results() const
{
    return m_results;
}

