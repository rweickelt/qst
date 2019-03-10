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

#include "exports.h"
#include "job.h"
#include "testcase.h"

#include <QtCore/QVector>

/* TestJobs are testcase-data tuples created by JobMultiplier
   and executed by TestRunner.
 */
struct JobData
{
    QMap<QString, QVariantList> dependenciesData;
    QString name;
    QString filePath;
    Testcase::Result result;
    TagSet tags;
    QVariantMap exports;
};

namespace {
    QVector<JobData> resources;
}

QMap<QString, QVariantList> Job::dependenciesData() const
{
    return resources[m_id].dependenciesData;
}

void Job::setDependenciesData(const QMap<QString, QVariantList>& data)
{
    resources[m_id].dependenciesData = data;
}

QVariantMap Job::exports() const
{
    return resources[m_id].exports;
}

void Job::setExports(const QVariantMap& data)
{
    resources[m_id].exports = data;
}

QString Job::filePath() const
{
    return resources[m_id].filePath;
}

QString Job::name() const
{
    return resources[m_id].name;
}

Testcase::Result Job::result() const
{
    return resources[m_id].result;
}

void Job::setResult(Testcase::Result result)
{
    resources[m_id].result = result;
}

TagSet Job::tags() const
{
    return resources[m_id].tags;
}

Job Job::create(Testcase* testcase, const TagSet& tags)
{
    JobData data;
    data.exports = testcase->exportsItem() ? testcase->exportsItem()->toVariantMap() : QVariantMap{};
    data.name = testcase->name();
    data.filePath = testcase->filepath();
    data.tags = tags;
    resources.append(data);

    Job job;
    job.m_id = resources.length() - 1;
    return job;
}
