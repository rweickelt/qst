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

#include "jobserver.h"
#include "projectdatabase.h"

JobServer::JobServer(ProjectDatabase* db, QObject* parent)
    : QObject(parent)
    , m_db(db)
{
    for (const auto& job : m_db->jobGraph.nodes()) {
        int nrOfDependentJobs = m_db->jobGraph.predecessors(job).size();
        m_dependencyCounts.insert(job, nrOfDependentJobs);
    }
}

void JobServer::onJobFinished(Job job)
{
    m_runningJobs.remove(job);
    m_doneJobs.insert(job);

    // Give resources back
    auto lockedResources = m_resourcesLockedByJob.values(job).toSet();
    m_lockedResources -= lockedResources;
    for (const auto& resource : lockedResources) {
        m_jobsPerLockedResource.remove(resource);
    }

    // Todo: skip dependent jobs if this one fails

    // Check whether a new job can be staged.
    for (Job& nextJob : m_db->jobGraph.successors(job)) {
        int nrOfPredecessors = m_dependencyCounts.value(nextJob);
        if (nrOfPredecessors > 0) {
            nrOfPredecessors -= 1;
            m_dependencyCounts[nextJob] = nrOfPredecessors;
        }
        if (nrOfPredecessors == 0) {
            Q_ASSERT(m_todoJobs.contains(nextJob));
            m_todoJobs.remove(nextJob);
            m_stagingJobs.insert(nextJob);
        }
    }

    checkStagedJobs();

    if (m_todoJobs.isEmpty() && m_stagingJobs.isEmpty() && m_runningJobs.isEmpty()) {
        emit finished();
    }
}

void JobServer::start()
{
    if (m_db->jobGraph.isEmpty()) {
        emit finished();
        return;
    }

    m_stagingJobs = m_db->jobGraph.roots().toSet();
    m_todoJobs = m_db->jobGraph.nodes().toSet() - m_stagingJobs;

    checkStagedJobs();

    Q_ASSERT(!m_runningJobs.isEmpty());
}

void JobServer::checkStagedJobs()
{
    for (auto nextJob : QSet<Job>(m_stagingJobs)) {
        bool jobCanRun = true;

        // Changes on these data structures will be committed back to their
        // m_xxx equivalences once the job can run
        QMultiMap<Job, Resource> resourcesLockedByJob = m_resourcesLockedByJob;
        QMultiMap<Resource, Job> jobsPerLockedResource = m_jobsPerLockedResource;
        QSet<Resource> lockedResources = m_lockedResources;
        QMap<Dependency, QSet<Resource> > resourceMapping;

        // Go through all dependencies and try to acquire sufficient resources
        for (const auto& mapping : m_db->eligibleResourcesPerJob.values(nextJob)) {
            const Dependency& dependency = mapping.first;
            const QSet<Resource>& eligibleResources = mapping.second;

            // We assume that eligible resources can always be used
            // unless they are locked
            QSet<Resource> availableResources = eligibleResources - lockedResources;

            if (availableResources.size() <= dependency.count()) {
                jobCanRun = false;
                break;
            }

            // Sufficient resources available for this dependency, now acquire them
            // and attach them to the job.
            QSet<Resource>& selectedResources = resourceMapping[dependency];
            int maxCount = dependency.count() > 0 ? dependency.count() : 1;
            for (int count = 0; count < maxCount; count++) {
                auto i = availableResources.begin();

                selectedResources.insert(*i);
                resourcesLockedByJob.insert(nextJob, *i);
                jobsPerLockedResource.insert(*i, nextJob);
                lockedResources.insert(*i);

                availableResources.erase(i);
            }
        }

        if (jobCanRun == false) {
            continue;
        }

        // Load exported data from preceding jobs
        for (const auto& predecessor : m_db->jobGraph.predecessors(nextJob)) {
            if (predecessor.exports().isEmpty()) {
                continue;
            }

            Dependency dependency = m_db->jobGraph.edge(predecessor, nextJob);
            QString alias = dependency.alias().isEmpty() ? dependency.name() : dependency.alias();
            nextJob.insertDependencyData(alias, predecessor.exports());
        }

        // Attach resource data
        // Todo: Handle read/write dependencies
        for (auto i = resourceMapping.constBegin(); i != resourceMapping.constEnd(); i++) {
            const Dependency& dependency = i.key();
            const QSet<Resource>& resources = i.value();

            QString alias = dependency.alias().isEmpty() ? dependency.name() : dependency.alias();

            for (const auto& resource : resources) {
                nextJob.insertDependencyData(alias, resource.data());
            }
        }

        // Commit
        m_resourcesLockedByJob = m_resourcesLockedByJob.unite(resourcesLockedByJob);
        m_jobsPerLockedResource = m_jobsPerLockedResource.unite(jobsPerLockedResource);
        m_lockedResources = m_lockedResources.unite(lockedResources);

        m_runningJobs.insert(nextJob);
        m_stagingJobs.remove(nextJob);
        emit jobReady(nextJob);
    }
}
