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

#ifndef MATRIXEXPANDER_H
#define MATRIXEXPANDER_H

#include "dimension.h"
#include "tag.h"
#include "job.h"

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QVariantMap>

class Matrix;
class QstDocument;
class Testcase;

/* JobExpander:
     - takes a list of matrix descriptions and expands them
       to a 1-dimensional vector of property tag combinations.

     - assigns tags to matching testcases based on the testcases
       property in Matrix and creates fixtures for later execution.

     - ensures that a test case is only part in max one matrix

     - property declarations in testcases match property tags in matrices

   JobExpander assumes that:

     - input data in dimensions has been tested for plausability
       (correct format, valid trivial values, no objecs etc.)

 */
class JobMultiplier : private QstItemVisitor
{
    Q_DISABLE_COPY(JobMultiplier)

public:
    JobMultiplier(const QList<QstDocument*>& documents);
    QString errorString() const;
    bool hasError() const;
    JobTable jobs() const;
    TagLookupTable tags() const;


private:
    // Expands matrix with n dimensions into 1-dimensional vector with n columns
    static QMap<TagId, TagSet> expand(const Matrix* matrix);

    // Matches existing testcases against wildcard patterns
    static QStringList match(const QStringList& testcases, const QStringList& patterns);

    // Creates a job table from a list of testcases and tags
    QMultiMap<QString, Job> combine(const QStringList& testcases,
                                        const QMap<TagId, TagSet>& tags);

    // Returns a filtered jobs table with all exceptions removed
    static QMultiMap<QString, Job> removeExcluded(const QMultiMap<QString, Job>& jobs,
                                                      const QStringList& patterns,       // = "Excepts" item
                                                      const TagLookupTable& tags); // = "Excepts" item

    virtual void visit(Matrix* item) final;
    virtual void visit(Testcase* item) final;

    QString m_errorString;
    QList<Matrix*> m_matrices;
    QMap<QString, Testcase*> m_testcases;
    JobTable m_jobs;
    TagLookupTable m_tags;
};

inline QString JobMultiplier::errorString() const { return m_errorString; }
inline bool JobMultiplier::hasError() const { return !m_errorString.isEmpty(); }
inline JobTable JobMultiplier::jobs() const { return m_jobs; }
inline TagLookupTable JobMultiplier::tags() const { return m_tags; }


#endif // MATRIXEXPANDER_H
