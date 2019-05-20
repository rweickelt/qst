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
#include "testcase.h"

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QString>

class Exports;
class ProjectDatabase;

/*
Creates a job schedule and supervises execution by a dispatcher.
The schedule is a topological order of the dependency graph.

This scheduler executes jobs one-by-one.
 */
class SerialJobScheduler : public QObject
{
    Q_OBJECT
public:
    SerialJobScheduler(ProjectDatabase* db, QObject* parent = nullptr);
    void start();
    QList<Testcase::Result> results() const;

public slots:
    void onJobFinished(Job job);

signals:
    void finished();
    void jobReady(Job job);

private:

    ProjectDatabase* m_db;
    QMap<Job, int> m_dependencyCounts;
    QList<Job> m_done;
    QList<Job> m_readyList;
    QList<Testcase::Result> m_results;
    QList<Job> m_todo;
};

#endif // SERIALJOBSCHEDULER_H
