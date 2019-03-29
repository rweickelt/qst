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

#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtQml/QQmlEngine>

class ProjectDatabase;

class JobDispatcher : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(JobDispatcher)
public:
    JobDispatcher(const ProjectDatabase& db);
    ~JobDispatcher();

public slots:
    void dispatch(Job job);

signals:
    void finished(Job job);

protected slots:

private:
    void run();
//    static QVariantMap parseExports(Exports* item);

    const ProjectDatabase& m_db;
};

#endif // JOBRUNNER_H
