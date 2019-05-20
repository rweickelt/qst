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

#include "matrixresolver.h"
#include "matrix.h"
#include "project.h"
#include "qst.h"
#include "qstdocument.h"
#include "resource.h"
#include "resourceitem.h"
#include "testcase.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>
#include <QtCore/QStringList>

#include <QtDebug>

MatrixResolver::MatrixResolver(const QList<QstDocument*>& documents)
{
    for (const auto& document : documents) {
        document->object->accept(this);
    }

    auto duplicates = m_testcases.keys().toSet().intersect(m_resourceItems.keys().toSet());
    for (const auto& name : duplicates) {
        QmlContext contextT = qst::qmlDefinitionContext(m_testcases[name]);
        QmlContext contextR = qst::qmlDefinitionContext(m_resourceItems.value(name));
        QString error = QString("At %1:%2: name '%3' shadows Testcase at %4:%5.")
                .arg(contextR.file())
                .arg(contextR.line())
                .arg(name)
                .arg(contextT.file())
                .arg(contextT.line());
        m_errorStrings << error;
    }

    //
    // Create tagged jobs for all test cases that
    // are part of a matrix.
    //
    for (const auto& matrix : m_matrices) {
        QList<TagSet> tags = expand(matrix);
        QStringList patterns = matrix->names();
        QSet<QString> matchedNames = match(m_items.keys(), patterns).toSet();

        if (matchedNames.isEmpty()) {
            QmlContext context = qst::qmlDefinitionContext(matrix);
            QST_ERROR_AND_EXIT(QString("Matrix defined at %1:%2 does not match any testaces or resources.")
                                   .arg(context.file())
                                   .arg(context.line()));
        }

        QMultiMap<QString, TagSet> combined;
        QSet<QString> selectedMatches;

        //
        // Testcases
        //
        selectedMatches = m_testcases.keys().toSet().intersect(matchedNames);
        combined = combine(selectedMatches, tags);
        for (auto i = combined.constBegin(); i != combined.constEnd(); i++) {
            QString name = i.key();
            Job job = Job::create(m_testcases[name], i.value());
            m_jobs.insert(name, job);
        }

        //
        // Resources
        //
        selectedMatches = m_resourceItems.keys().toSet().intersect(matchedNames);
        combined = combine(selectedMatches, tags);
        for (auto i = combined.constBegin(); i != combined.constEnd(); i++) {
            QString name = i.key();
            for (const auto& item: m_resourceItems.values(name))
            {
                Resource resource = Resource::create(item, i.value());
                m_resources.insert(name, resource);
            }
        }
    }

    QSet<QString> allNames;
    QSet<QString> usedNames;
    QSet<QString> untaggedNames;

    //
    // Turn untagged Testcase items into jobs
    //
    allNames = m_testcases.keys().toSet();
    usedNames = m_jobs.uniqueKeys().toSet();
    untaggedNames = allNames.subtract(usedNames);
    for (const auto& name : untaggedNames) {
        Q_ASSERT(!m_jobs.contains(name));
        m_jobs.insert(name, Job::create(m_testcases[name], TagSet()));
    }

    //
    // Turn untagged Resource items into resources
    //
    allNames = m_resourceItems.keys().toSet();
    usedNames = m_resources.uniqueKeys().toSet();
    untaggedNames = allNames.subtract(usedNames);
    for (const auto& name : untaggedNames) {
        Q_ASSERT(!m_resources.contains(name));
        for (const auto& item: m_resourceItems.values(name))
        {
            m_resources.insert(name, Resource::create(item, TagSet()));
        }
    }
}

/*
   Span an array for job data. Multiple dimensions are defined in QML,
   but we flatten everything into a vector.
   Think of this process as creating a numbers with m digits where m is
   the number of dimensions and each digit goes from 0 - size(dimension).

   We assume that dimensions always have at least 1 entry.
*/
QList<TagSet> MatrixResolver::expand(const Matrix* matrix)
{
    QList<int> lengths;
    QList<int> dividers;
    QSet<QString> tagnames;

    //
    // Find out the resulting length of the flattended vector
    // and calculate dividers for later index calculation.
    //
    for (int i = 0; i < matrix->dimensions().length(); i++) {
        const Dimension* dimension = matrix->dimensions()[i];

        int length = dimension->length();
        Q_ASSERT(length > 0);

        lengths << length;
        dividers << dividers.value(i - 1, 1) * lengths.value(i - 1, 1);

        QSet<QString> dimensionTagnames = dimension->data().first().keys().toSet();
        if (tagnames.intersects(dimensionTagnames)) {
            QmlContext context = qst::qmlDefinitionContext(matrix);
            QST_ERROR_AND_EXIT(QString("Dimensions in Matrix defined %1:%2 have overlapping tags.")
                                   .arg(context.file())
                                   .arg(context.line()));
        }
        tagnames += dimensionTagnames;
    }

    QList<TagSet> expandedTags;
    for (int i = 0; i < dividers.last() * lengths.last(); i++) {
        TagSet tagsPerJob;
        for (int j = 0; j < matrix->dimensions().length(); j++) {
            const Dimension* dimension = matrix->dimensions()[j];
            QVariantMap data = dimension->data()[(i / dividers[j]) % lengths[j]];
            for (const auto& key : data.keys()) {
                tagsPerJob << Tag::create(key, data.value(key).toString());
            }
        }
        expandedTags << tagsPerJob;
    }

    return expandedTags;
}

/* We can expect that testcase names and patterns have been sanitized. */
QStringList MatrixResolver::match(const QStringList& testcases, const QStringList& patterns)
{
    QStringList matchedNames;
    QStringList regifiedPatterns = patterns;
    regifiedPatterns.replaceInStrings("*", ".*");
    regifiedPatterns.replaceInStrings("?", ".");

    //
    // Let's combine everything in one big regexp and assume that
    // the regexp implementation is fast enough.
    //
    QRegularExpression pattern("^(" + regifiedPatterns.join('|') + ")$",
        QRegularExpression::OptimizeOnFirstUsageOption);

    for (const auto& name : testcases) {
        QRegularExpressionMatch match = pattern.match(name);
        if (match.hasMatch()) {
            matchedNames << name;
        }
    }

    return matchedNames;
}

QMultiMap<QString, TagSet> MatrixResolver::combine(const QSet<QString>& names, const QList<TagSet>& tagsets)
{
    QMultiMap<QString, TagSet> result;

    for (const auto& name : names) {
        Testcase* testcase = m_testcases[name];

        //
        // We assume that every TagList in tags has identical tag keys.
        // Thus it is safe to test compatibility with the first TagList only.
        //
        Q_ASSERT(tagsets.length() > 0);
        for (const auto& representative : tagsets.first()) {
            QString label = representative.label();
            QQmlProperty property(testcase, label);
            if (!property.isProperty()) {
                QmlContext context = qst::qmlDefinitionContext(testcase);
                QST_ERROR_AND_EXIT(QString("Item defined at %1:%2 does not have a property with name '%3'")
                                       .arg(context.file())
                                       .arg(context.line())
                                       .arg(label));
            }
            if (!property.isWritable()) {
                QmlContext context = qst::qmlDefinitionContext(testcase);
                QST_ERROR_AND_EXIT(QString("Property '%3' of item '%4' at %1:%2 is not writable.")
                                       .arg(context.file())
                                       .arg(context.line())
                                       .arg(label)
                                       .arg(name));
            }
        }

        for (const auto& tagset : tagsets) {
            result.insert(name, tagset);
        }
    }
    return result;
}

void MatrixResolver::visit(ResourceItem* item)
{
    m_resourceItems.insert(item->name(), item);
    m_items.insert(item->name(), item);
}

void MatrixResolver::visit(Testcase* item)
{
    m_items.insert(item->name(), item);
    m_testcases[item->name()] = item;
}

void MatrixResolver::visit(Matrix* item)
{
    m_matrices.append(item);
}
