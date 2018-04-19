#ifndef MATRIXEXPANDER_H
#define MATRIXEXPANDER_H

#include "dimension.h"
#include "testjob.h"

#include <QtCore/QList>
#include <QtCore/QVariantMap>

#include <tuple>

class Matrix;
class Testcase;

/* JobExpander:
     - takes a list of matrix descriptions and expands them
       to a 1-dimensional vector of property tag combinations.

     - assigns tags to matching testcases based on the testcases
       property in Matrix and creates fixtures for later execution.

     - ensures that a test case is only part in one matrix

     - property declarations in testcases match property tags in matrices

   JobExpander assumes that:

     - input data in dimensions has been tested for plausability
       (correct format, valid trivial values, no objecs etc.)

 */
class JobExpander
{
    Q_DISABLE_COPY(JobExpander)

public:
    JobExpander(const QList<Matrix*>& matrices, const QList<Testcase*>& testcases);
    QString errorString() const;
    bool hasError() const;
    QMultiMap<QString, TestJob> jobs() const;
    QVector<QVariantMap> tags() const;


private:
    // Expands matrix with n dimensions into 1-dimensional vector with n columns
    static QVector<QVariantMap> expand(const Matrix* matrix);

    // Matches existing testcases against wildcard patterns
    static QStringList match(const QStringList& testcases, const QStringList& patterns);

    // Creates a job table from a list of testcases and tags
    QMultiMap<QString, TestJob> combine(const QStringList& testcases,
                                        const QVector<QVariantMap>& tags);

    // Returns a filtered jobs table with all exceptions removed
    static QMultiMap<QString, TestJob> filterExempted(const QMultiMap<QString, TestJob>& jobs,
                                                      const QStringList& patterns,       // = "Excepts" item
                                                      const QVector<QVariantMap>& tags); // = "Excepts" item


    static bool isValid(const Matrix& matrix);
    static bool isValid(const Dimension& dimension);

    QString m_errorString;
    QList<Matrix*> m_matrices;
    QMap<QString, Testcase*> m_testcases;
    QMultiMap<QString, TestJob> m_jobs;

    QVector<QVariantMap> m_tags;
};

inline QString JobExpander::errorString() const { return m_errorString; }
inline bool JobExpander::hasError() const { return !m_errorString.isEmpty(); }
inline QMultiMap<QString, TestJob> JobExpander::jobs() const { return m_jobs; }
inline QVector<QVariantMap> JobExpander::tags() const { return m_tags; }


#endif // MATRIXEXPANDER_H
