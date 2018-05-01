#ifndef MATRIXEXPANDER_H
#define MATRIXEXPANDER_H

#include "dimension.h"
#include "tag.h"
#include "testjob.h"

#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtCore/QVariantMap>

class Matrix;
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
class JobMultiplier
{
    Q_DISABLE_COPY(JobMultiplier)

public:
    JobMultiplier(const QList<Matrix*>& matrices, const QList<Testcase*>& testcases);
    QString errorString() const;
    bool hasError() const;
    QMultiMap<QString, TestJob> jobs() const;
    TagLookupTable tags() const;


private:
    // Expands matrix with n dimensions into 1-dimensional vector with n columns
    static QMap<TagId, Tag> expand(const Matrix* matrix);

    // Matches existing testcases against wildcard patterns
    static QStringList match(const QStringList& testcases, const QStringList& patterns);

    // Creates a job table from a list of testcases and tags
    QMultiMap<QString, TestJob> combine(const QStringList& testcases,
                                        const QMap<TagId, Tag>& tags);

    // Returns a filtered jobs table with all exceptions removed
    static QMultiMap<QString, TestJob> removeExcluded(const QMultiMap<QString, TestJob>& jobs,
                                                      const QStringList& patterns,       // = "Excepts" item
                                                      const TagLookupTable& tags); // = "Excepts" item

    QString m_errorString;
    QList<Matrix*> m_matrices;
    QMap<QString, Testcase*> m_testcases;
    QMultiMap<QString, TestJob> m_jobs;
    TagLookupTable m_tags;
};

inline QString JobMultiplier::errorString() const { return m_errorString; }
inline bool JobMultiplier::hasError() const { return !m_errorString.isEmpty(); }
inline QMultiMap<QString, TestJob> JobMultiplier::jobs() const { return m_jobs; }
inline TagLookupTable JobMultiplier::tags() const { return m_tags; }


#endif // MATRIXEXPANDER_H
