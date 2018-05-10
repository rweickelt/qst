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

#ifndef TESTJOB_H
#define TESTJOB_H

#include "tag.h"

#include <QtCore/QSharedData>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QVariantMap>

class Testcase;

struct Job;

using JobTable = QMultiMap<QString, Job>;


/* TestJobs are testcase-data tuples created by MatrixExpander
   and executed by TestRunner.
 */
struct JobData : public QSharedData
{
    Testcase* testcase;
    TagGroupId tagGroupId;
    TagId tagId;
};

class Job
{
public:
    Job (Testcase* testcase = nullptr, TagGroupId tagGroupId = InvalidId, TagId tagId = InvalidId);
    TagGroupId tagGroupId() const { return d->tagGroupId; }
    TagId tagId() const { return d->tagId; }
    Testcase* testcase() { return d->testcase; }
    Testcase* testcase() const { return d->testcase; }

private:
    QSharedDataPointer<JobData> d;
};

#endif // TESTJOB_H
