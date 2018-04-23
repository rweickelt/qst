/****************************************************************************
 **
 ** Copyright (C) 2017, 2018 The Qst project.
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
#ifndef JOBRUNNER_H
#define JOBRUNNER_H

#include "tag.h"
#include "testcase.h"
#include "testjob.h"

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QVariantMap>

class Project;

class JobRunner
{
    Q_DISABLE_COPY(JobRunner)
public:
    JobRunner(Project* project, const QList<TestJob>& testCases, const TagStorage& tags);
    QString errorString() const;
    void execTestCases();
    bool hasError() const;

private:
    void createProjectWorkingDirectory();
    QString createTestcaseWorkingDirectory(const QString& name);

    QString m_errorString;
    Project* m_project;
    QList<TestJob> m_jobs;
    QList<Testcase::Result> m_results;
    TagStorage m_tags;
};

inline QString JobRunner::errorString() const { return m_errorString; }
inline bool JobRunner::hasError() const { return !m_errorString.isEmpty(); }

#endif // JOBRUNNER_H
