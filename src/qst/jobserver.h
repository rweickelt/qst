/****************************************************************************
 **
 ** Copyright (C) 2019 The Qst project.
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
#ifndef JOBSERVER_H
#define JOBSERVER_H

#include "job.h"
#include "resource.h"
#include "testcase.h"

#include <QtCore/QObject>

class ProjectDatabase;

/*
Triggers jobs according to their dependencies. Up to N parallel jobs may be
triggered in parallel.
 */
class JobServer : public QObject {
    Q_OBJECT
public:
    JobServer(ProjectDatabase* db, QObject* parent = nullptr);
    void start();

public slots:
    void onJobFinished(Job job);

signals:
    void finished();
    void jobReady(Job job);

private:
    void checkStagedJobs();

    ProjectDatabase* m_db;
    QSet<Job> m_todoJobs;    // unprocessed jobs
    QSet<Job> m_stagingJobs; // ready to run, but missing resources
    QSet<Job> m_runningJobs;
    QSet<Job> m_doneJobs;    // finished jobs
    QMultiMap<Job, Resource> m_resourcesLockedByJob;
    QMultiMap<Resource, Job> m_jobsPerLockedResource;
    QSet<Resource> m_lockedResources;
    QMap<Job, int> m_dependencyCounts;
};

#endif // JOBSERVER_H
