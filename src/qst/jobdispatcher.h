/****************************************************************************
 **
 ** Copyright (C) 2017-2019 The Qst project.
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

#include "job.h"
#include "testcase.h"

#include <QtCore/QDir>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QVariantMap>

class Project;

class JobDispatcher : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(JobDispatcher)
public:
    JobDispatcher(const QVariantMap& project);
    QList<Testcase::Result> results() const;

public slots:
    void dispatch(Job job);

signals:
    void errorOccured(QString message);
    void finished(Job job);

private:
    QString createTestcaseWorkingDirectory(const QString& name);

    QVariantMap m_project;
    QDir m_projectWorkingDirectory;
    QList<Testcase::Result> m_results;
};

inline QList<Testcase::Result> JobDispatcher::results() const { return m_results; }

#endif // JOBRUNNER_H
