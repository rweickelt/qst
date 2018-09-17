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

#include "job.h"

#include <QtCore/QVector>

namespace {
//    class JobTable
//    {

//    public:
//        void insert(const Job& job);
//        Job value(const QString& name, const QList<Tag>& tags);
//        QList<Job> values(const QString& name, const QList<Tag>& tags);

//    private:
//        QVector<Job> m_jobs;
//    };

//    JobTable jobs;

    QVector<JobData> jobs;
}

QVariantMap Job::exports() const
{
    return jobs[m_id].exports;
}

void Job::setExports(const QVariantMap& data)
{
    jobs[m_id].exports = data;
}

TagSet Job::tags() const
{
    return jobs[m_id].tags;
}

Testcase* Job::testcase()
{
    return jobs[m_id].testcase;
}

Testcase* Job::testcase() const
{
    return jobs[m_id].testcase;
}

Job Job::create(Testcase* testcase, const TagSet& tags)
{
    JobData data;
    data.testcase = testcase;
    data.tags = tags;
    jobs.append(data);

    Job job;
    job.m_id = jobs.length() - 1;
    return job;
}
