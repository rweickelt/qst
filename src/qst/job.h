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

#ifndef JOB_H
#define JOB_H

#include "tag.h"
#include "testcase.h"

#include <QtCore/QList>
#include <QtCore/QMultiMap>
#include <QtCore/QVariantMap>

class Testcase;

struct Job;

using JobTable = QMultiMap<QString, Job>;

class Job
{
public:
    using Id = int;
    enum { InvalidId = -1 };

    QMap<QString, QVariantList> dependenciesData() const;
    void setDependenciesData(const QMap<QString, QVariantList>& data);
    bool isValid() const;
    Id id() const;
    QString filePath() const;
    QString name() const;
    Testcase::Result result() const;
    TagSet tags() const;
    Testcase* testcase();
    Testcase* testcase() const;
    QVariantMap exports() const;
    void setExports(const QVariantMap& data);
    void setResult(Testcase::Result result);

    bool operator==(const Job& other) const;
    bool operator<(const Job& other) const;

    static Job create(Testcase* testcase, const TagSet& tags = TagSet());

    static QList<Job> find(const QString& name, const QList<Tag>& tags);

private:
    Id m_id = InvalidId;
};

Q_DECLARE_METATYPE(Job)

uint qHash(const Job& job);

inline bool Job::isValid() const { return m_id != InvalidId; }
inline Job::Id Job::id() const { return m_id; }
inline bool Job::operator==(const Job& other) const { return (m_id == other.m_id); }
inline bool Job::operator<(const Job& other) const { return (m_id < other.m_id); }

inline uint qHash(const Job& job) { return job.id(); }

#endif // JOB_H
