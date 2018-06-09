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

#ifndef SERIALJOBSCHEDULER_H
#define SERIALJOBSCHEDULER_H

#include "dependency.h"
#include "directedgraph.h"
#include "job.h"

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QString>

class Exports;

/*
Creates a job schedule and supervises execution by a dispatcher.
The schedule is a topological order of the dependency graph.

This scheduler executes jobs one-by-one.
 */
class SerialJobScheduler : public QObject
{
    Q_OBJECT
public:
    SerialJobScheduler(const DirectedGraph<Job, Dependency>& jobs, QObject* parent = nullptr);
    void start();

public slots:
    void onJobFinished(const Job& job);

signals:
    void finished();
    void jobReady(const Job& job);

private:
    static QVariantMap parseExports(Exports* item);

    JobList m_todo;
    JobList m_done;
    DirectedGraph<Job, Dependency> m_dependencies;
    QMap<Job, uint> m_dependencyCounts;
    JobList m_readyList;
};

#endif // SERIALJOBSCHEDULER_H
