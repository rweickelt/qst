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
#ifndef JOBDISPATCHER_H
#define JOBDISPATCHER_H

#include "testcase.h"
#include "job.h"

#include <QtCore/QDir>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QVariantMap>

class Project;

class JobDispatcher : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(JobDispatcher)
public:
    JobDispatcher(const QVariantMap& project, const QString& workDirPath);
    QString errorString() const;
    bool hasError() const;
    QList<Testcase::Result> results() const;

public slots:
    void dispatch(Job job);

signals:
    void finished(Job);

private:
    void createProjectWorkingDirectory(const QString& dirPath);
    QString createTestcaseWorkingDirectory(const QString& name);

    QString m_errorString;
    QVariantMap m_project;
    QList<Testcase::Result> m_results;
    QDir m_workingDirectory;
};

inline QString JobDispatcher::errorString() const { return m_errorString; }
inline bool JobDispatcher::hasError() const { return !m_errorString.isEmpty(); }
inline QList<Testcase::Result> JobDispatcher::results() const { return m_results; }

#endif // JOBRUNNER_H
