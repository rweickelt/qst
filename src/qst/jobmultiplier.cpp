
#include "jobmultiplier.h"
#include "matrix.h"
#include "project.h"
#include "qst.h"
#include "testcase.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>
#include <QtCore/QStringList>

#include <QtDebug>

namespace {

    template<typename KeyT, typename ValueT>
    QMap<KeyT, ValueT>& operator+=(QMap<KeyT, ValueT>& self, const QMap<KeyT, ValueT>& other)
    {
        for (const auto& key: other.keys())
        {
            Q_ASSERT(!self.contains(key));
            self.insert(key, other[key]);
        }
        return self;
    }

}

JobMultiplier::JobMultiplier(const QList<Matrix*>& matrices, const QList<Testcase*>& testcases) :
    m_matrices(matrices)
{
    for (const auto& testcase: testcases)
    {
        m_testcases[testcase->name()] = testcase;
    }

    // Create tagged jobs for all test cases that
    // are part of a matrix.
    for (const auto& matrix: m_matrices)
    {
        QMap<TagId, Tag> tags = expand(matrix);
        QStringList patterns = matrix->testcases();
        QStringList names = match(m_testcases.keys(), patterns);

        if (names.isEmpty())
        {
            QmlContext context = qst::qmlDefinitionContext(matrix);
            QST_ERROR_AND_EXIT(QString("Matrix defined at %1:%2 doesn't match any testcase.")
                               .arg(context.file()).arg(context.line()));
        }

//        QSet<QString> usedNames = m_jobs.uniqueKeys().toSet();
//        QSet<QString> overlappingNames = names.toSet().intersect(usedNames);
//        if (overlappingNames.size() > 0)
//        {
//            QmlContext context = qst::qmlDefinitionContext(matrix);
//            QST_ERROR_AND_EXIT(QString("Testcases ('%1') in matrix defined at %2:%3 occur in multiple matrices. "
//                                       "Each testcase can only occur in one matrix. Check the 'testcases' property.")
//                               .arg(overlappingNames.toList().join("', '"))
//                               .arg(context.file()).arg(context.line()));
//        }

        QMultiMap<QString, TestJob> jobs = combine(names, tags);
        jobs = removeExcluded(jobs, QStringList(), TagLookupTable());

        m_jobs += jobs;
        TagGroupId groupId = makeTagGroupId(tags.first());
        if (m_tags.contains(groupId))
        {
            m_tags[groupId] += tags;
        }
        else
        {
            m_tags[groupId] = tags;
        }
    }

    QSet<QString> allNames = m_testcases.keys().toSet();
    QSet<QString> usedNames = m_jobs.uniqueKeys().toSet();
    QSet<QString> untaggedNames = allNames.subtract(usedNames);

    // Create normal jobs for the remaining test cases.
    for (const auto& name: untaggedNames)
    {
        Q_ASSERT(!m_jobs.contains(name));
        m_jobs.insert(name, TestJob::fromTestcase(m_testcases[name]));
    }
}


/*
   Span an array for job data. Multiple dimensions are defined in QML,
   but we flatten everything into a vector.
   Think of this process as creating a numbers with m digits where m is
   the number of dimensions and each digit goes from 0 - size(dimension).

   We assume that dimensions always have at least 1 entry.
*/
QMap<TagId, Tag> JobMultiplier::expand(const Matrix* matrix)
{
    QList<int> lengths;
    QList<int> dividers;
    QSet<QString> tagnames;

    // Find out the resulting length of the flattended vector
    // and calculate dividers for later index calculation.
    for (int i = 0; i < matrix->dimensions().length(); i++)
    {
        const Dimension* dimension = matrix->dimensions()[i];

        int length = dimension->length();
        Q_ASSERT(length > 0);

        lengths << length;
        dividers << dividers.value(i-1, 1) * lengths.value(i-1, 1);

        QSet<QString> dimensionTagnames = dimension->data().first().keys().toSet();
        if (tagnames.intersects(dimensionTagnames))
        {
            QmlContext context = qst::qmlDefinitionContext(matrix);
            QST_ERROR_AND_EXIT(QString("Dimensions in Matrix defined %1:%2 have overlapping tags.")
                               .arg(context.file()).arg(context.line()));
        }
        tagnames += dimensionTagnames;
    }

    QMap<TagId, Tag> expandedTags;
    for (int i = 0; i < dividers.last() * lengths.last(); i++)
    {
        Tag tag;

        for (int j = 0; j < matrix->dimensions().length(); j++)
        {
            const Dimension* dimension = matrix->dimensions()[j];
            tag += dimension->data()[(i / dividers[j]) % lengths[j]];
        }

        TagId id = makeTagId(tag);
        Q_ASSERT(!expandedTags.contains(id));
        expandedTags[id] = tag;
    }

    return expandedTags;
}

/* We can expect that testcase names and patterns have been sanitized. */
QStringList JobMultiplier::match(const QStringList& testcases, const QStringList& patterns)
{
    QStringList matchedNames;
    QStringList regifiedPatterns = patterns;
    regifiedPatterns.replaceInStrings("*", ".*");
    regifiedPatterns.replaceInStrings("?", ".");

    // Let's combine everything in one big regexp and assume that
    // the regexp implementation is fast enough.
    QRegularExpression pattern("^(" + regifiedPatterns.join('|') + ")$",
                               QRegularExpression::OptimizeOnFirstUsageOption);

    for (const auto& name: testcases)
    {
        QRegularExpressionMatch match = pattern.match(name);
        if (match.hasMatch())
        {
            matchedNames << name;
        }
    }

    return matchedNames;
}

QMultiMap<QString, TestJob> JobMultiplier::combine(const QStringList& testcases, const QMap<TagId, Tag>& tags)
{
    QMultiMap<QString, TestJob> result;
    for (const auto& name: testcases)
    {
        Testcase* testcase = m_testcases[name];

        for (const auto& name: tags.first().keys())
        {
            QQmlProperty property(testcase, name);
            if (!property.isProperty())
            {
                QmlContext context = qst::qmlDefinitionContext(testcase);
                QST_ERROR_AND_EXIT(QString("Testcase defined at %1:%2 does not have a property with name '%3'")
                                   .arg(context.file()).arg(context.line()).arg(name));
            }
            if (!property.isWritable())
            {
                QmlContext context = qst::qmlDefinitionContext(testcase);
                QST_ERROR_AND_EXIT(QString("Property '%3' of Testcase '%4' at %1:%2 is not writable.")
                                   .arg(context.file()).arg(context.line()).arg(name)
                                   .arg(testcase->name()));
            }
        }

        for (const auto& tag: tags)
        {
            TestJob job;
            job.testcase = testcase;
            job.tagGroupId = makeTagGroupId(tag);
            job.tagId = makeTagId(tag);
            result.insert(name, job);
        }
    }
    return result;
}


QMultiMap<QString, TestJob> JobMultiplier::removeExcluded(const QMultiMap<QString, TestJob>& jobs,
                                                          const QStringList& patterns,
                                                          const TagLookupTable& tags)
{
    Q_UNUSED(patterns);
    Q_UNUSED(tags);
    return jobs;
}

