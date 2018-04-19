
#include "jobexpander.h"
#include "matrix.h"
#include "project.h"
#include "qst.h"
#include "testcase.h"

#include <QtDebug>

namespace {

//    QVariantMap operator+(const QVariantMap& left, const QVariantMap& right)
//    {
//        QVariantMap result = left;
//        for (const auto& key: right.keys())
//        {
//            Q_ASSERT(!left.contains(key));
//            result.insert(key, right[key]);
//        }
//        return result;
//    }

    QVariantMap& operator+=(QVariantMap& self, const QVariantMap& other)
    {
        for (const auto& key: other.keys())
        {
            Q_ASSERT(!self.contains(key));
            self.insert(key, other[key]);
        }
        return self;
    }

}

JobExpander::JobExpander(const QList<Matrix*>& matrices, const QList<Testcase*>& testcases) :
    m_matrices(matrices)
{
    for (const auto& testcase: testcases)
    {
        m_testcases[testcase->name()] = testcase;
    }

    for (const auto& matrix: m_matrices)
    {
        QVector<QVariantMap> tags = expand(matrix);
        QStringList patterns = matrix->testcases();
        QStringList names = match(m_testcases.uniqueKeys(), patterns);
        QMultiMap<QString, TestJob> jobs = combine(names, tags);
        jobs = filterExempted(jobs, QStringList(), QVector<QVariantMap>());

        m_jobs += jobs;
        m_tags = tags;
    }
}

bool JobExpander::isValid(const Matrix& matrix)
{
//    QStringList properties;
//    for (const auto& dimension: matrix.dimensions())
//    {
//        for (const auto& name: dimension->data().first().keys())
//        {

//        }
//    }
    return true;
}

bool JobExpander::isValid(const Dimension& dimension)
{
    Q_UNUSED(dimension);
    return true;
}

/*
   Span an array for job data. Multiple dimensions are defined in QML,
   but we flatten everything into a vector.
   Think of this process as creating a numbers with m digits where m is
   the number of dimensions and each digit goes from 0 - size(dimension).

   We assume that dimensions always have at least 1 entry.
*/
QVector<QVariantMap> JobExpander::expand(const Matrix* matrix)
{
    QList<int> lengths;
    QList<int> dividers;

    // Find out the resulting length of the flattended vector
    // and calculate dividers for later index calculation.
    for (int i = 0; i < matrix->dimensions().length(); i++)
    {
        const auto& dimension = matrix->dimensions()[i];
        int length = dimension->length();
        Q_ASSERT(length > 0);

        lengths << length;
        dividers << dividers.value(i-1, 1) * lengths.value(i-1, 1);
    }

    QVector<QVariantMap> expandedJobs(dividers.last() * lengths.last());
    for (int i = 0; i < expandedJobs.length(); i++)
    {
        QVariantMap currentJob;

        for (int j = 0; j < matrix->dimensions().length(); j++)
        {
            const auto& dimension = matrix->dimensions()[j];
            currentJob += dimension->data()[(i / dividers[j]) % lengths[j]];
        }

        expandedJobs[i] = currentJob;
    }

    return expandedJobs;
}

QStringList JobExpander::match(const QStringList& testcases, const QStringList& patterns)
{
    return testcases;
}

QMultiMap<QString, TestJob> JobExpander::combine(const QStringList& testcases, const QVector<QVariantMap>& tags)
{
    QMultiMap<QString, TestJob> result;
    for (const auto& name: testcases)
    {
        for (int i = 0; i < tags.length(); i++)
        {
            TestJob job;
            job.testcase = m_testcases[name];
            job.tagEntry = i;
            result.insert(name, job);
        }
    }
    return result;
}


QMultiMap<QString, TestJob> JobExpander::filterExempted(const QMultiMap<QString, TestJob>& jobs,
                                                          const QStringList& patterns,
                                                          const QVector<QVariantMap>& tags)
{
    return jobs;
}

