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

#ifndef MATRIXRESOLVER_H
#define MATRIXRESOLVER_H

#include "dimension.h"
#include "job.h"
#include "resource.h"
#include "tag.h"

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QVariantMap>

class Matrix;
class ResourceItem;
class QstDocument;
class QstItem;

/* MatrixResolver:
     - takes a list of matrix descriptions and expands them
       to a 1-dimensional vector of property tag combinations.

     - assigns tags to matching items based on the names
       property in Matrix and creates fixtures for later execution.

     - ensures that an item is only part in max one matrix

     - property declarations in items match property tags in matrices

   MatrixResolver assumes that:

     - input data in dimensions has been tested for plausability
       (correct format, valid trivial values, no objecs etc.)

 */
class MatrixResolver : private QstItemVisitor {
    Q_DISABLE_COPY(MatrixResolver)

public:
    MatrixResolver(const QList<QstDocument*>& documents);
    QStringList errors() const;
    bool hasErrors() const;
    JobTable jobs() const;
    ResourceTable resources() const;
    QMap<QString, QList<TagSet> > taggedItems() const;

private:
    // Expands matrix with n dimensions into 1-dimensional vector with n columns
    static QList<TagSet> expand(const Matrix* matrix);

    // Matches existing known items against wildcard patterns
    static QStringList match(const QStringList& items, const QStringList& patterns);

    // Creates a look-up table from a list of named items and tags
    QMultiMap<QString, TagSet> combine(const QSet<QString>& names, const QList<TagSet>& tags);

    virtual void visit(Matrix* item) final;
    virtual void visit(ResourceItem* item) final;
    virtual void visit(Testcase* item) final;

    QStringList m_errorStrings;
    QList<Matrix*> m_matrices;
    QMap<QString, QstItem*> m_items;
    QMap<QString, ResourceItem*> m_resourceItems;
    QMap<QString, Testcase*> m_testcases;
    JobTable m_jobs;
    ResourceTable m_resources;
};

inline QStringList MatrixResolver::errors() const { return m_errorStrings; }
inline bool MatrixResolver::hasErrors() const { return !m_errorStrings.isEmpty(); }
inline JobTable MatrixResolver::jobs() const { return m_jobs; }
inline ResourceTable MatrixResolver::resources() const { return m_resources; }

#endif // MATRIXRESOLVER_H
