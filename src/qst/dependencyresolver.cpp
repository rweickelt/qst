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

#include "dependency.h"
#include "dependencyresolver.h"
#include "depends.h"
#include "exports.h"
#include "job.h"
#include "qst.h"
#include "qstdocument.h"
#include "qstitemvisitor.h"
#include "testcase.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QVariantMap>
#include <QtCore/QSet>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>

namespace {
    QRegularExpression jsIdentifierPattern = QRegularExpression("^[A-Za-z][\\w]*$");
}

class DependencyVisitor : public QstItemVisitor
{
    Q_DISABLE_COPY(DependencyVisitor)

public:
    DependencyVisitor(DependencyResolver&);

protected:
    virtual void visit(Depends* item) final;
    virtual void visit(Exports* item) final;
    virtual void visit(Testcase* item) final;

private:
    DependencyResolver& m_resolver;
    Testcase* m_currentTestcase = nullptr;
};

DependencyVisitor::DependencyVisitor(DependencyResolver& resolver)
    : m_resolver(resolver)
{
}

void DependencyVisitor::visit(Depends* item)
{
    m_resolver.m_testcaseGraph.insertEdge(item->name(), m_currentTestcase->name(), item);
}

void DependencyVisitor::visit(Exports* item)
{
    m_resolver.m_exports.insert(m_currentTestcase->name(), item);
}

void DependencyVisitor::visit(Testcase* item)
{
    m_currentTestcase = item;
    m_resolver.m_testcases.insert(item->name(), item);
    m_resolver.m_testcaseGraph.insertNode(item->name());
}

DependencyResolver::DependencyResolver()
{

}

void DependencyResolver::beginResolve(const QList<QstDocument*> &documents)
{
    if (documents.isEmpty())
    {
        return;
    }

    DependencyVisitor visitor(*this);
    for (auto& document: documents)
    {
        document->object->accept(&visitor);
    }

    QSet<QString> tcNames = m_testcaseGraph.nodes().toSet();

    // Basic sanity checks on Depends item
    for (const auto& depends: m_testcaseGraph.edges())
    {
        if (depends->name().isEmpty())
        {
            QmlContext context = qst::qmlDefinitionContext(depends);
            QString message = QString("%1:%2: The name must not be empty.")
                    .arg(context.file()).arg(context.line());
            m_errors << message;
        }
        else if ((!depends->alias().isEmpty()) && (!jsIdentifierPattern.match(depends->alias()).hasMatch()))
        {
            QmlContext context = qst::qmlDefinitionContext(depends);
            QString message = QString("%2:%3: The alias '%1' must be a valid JavaScript identifier.")
                    .arg(depends->alias())
                    .arg(context.file()).arg(context.line());
            m_errors << message;
        }
        else if ((depends->alias().isEmpty()) && (!jsIdentifierPattern.match(depends->name()).hasMatch()))
        {
            QmlContext context = qst::qmlDefinitionContext(depends);
            QString message = QString("%2:%3: The name '%1' must be a valid JavaScript identifier when no alias is defined.")
                    .arg(depends->name())
                    .arg(context.file()).arg(context.line());
            m_errors << message;
        }
        else if (!tcNames.contains(depends->name()))
        {
            QmlContext context = qst::qmlDefinitionContext(depends);
            QString message = QString("%1:%2: The name '%3' is not an existing testcase.")
                    .arg(context.file()).arg(context.line())
                    .arg(depends->name());
            m_errors << message;
        }
    }

    if (hasErrors())
    {
        return;
    }

    // Cycle-checking depth-first search lambda function
    QSet<QString> visitedNodes;
    std::function<Depends*(const QString&, const QString&, QSet<QString>&)> cyclicDependency =
            [&cyclicDependency, &visitedNodes, this](const QString& successor, const QString& node, QSet<QString>& markedNodes) -> Depends*
            {
                if (markedNodes.contains(node))
                {
                    return m_testcaseGraph.edge(node, successor);
                }

                markedNodes << node;

                if (visitedNodes.contains(node))
                {
                    return nullptr;
                }

                visitedNodes << node;

                for (const auto& nextNode: m_testcaseGraph.predecessors(node))
                {
                    return cyclicDependency(node, nextNode, markedNodes);
                }

                return nullptr;
            };

    // Test graph for cycles.
    for (const auto& node: m_testcaseGraph.nodes())
    {
        QSet<QString> markedNodes;
        Depends* dependsItem = cyclicDependency(QString(), node, markedNodes);
        if (dependsItem != nullptr)
        {
            QmlContext context = qst::qmlDefinitionContext(dependsItem);
            QString message = QString("%2:%3: Cyclic dependency detected.")
                    .arg(context.file()).arg(context.line());
            m_errors << message;
        }
    }

    if (hasErrors())
    {
        return;
    }

    // Attach preliminary exports item to all dependents so that
    // QML engine can create bindings in dependents.
    for (const auto& tcName: tcNames)
    {
        Exports* exportItem = m_exports.value(tcName);
        if (!exportItem)
        {
            continue;
        }

        // Put the exports item into a list because we do that
        // later for all dependencies as well.
        QJsonArray exports;
        exports.append(QJsonValue::fromVariant(exportItem->toVariantMap()));

        for (const auto& dependentTcName: m_testcaseGraph.successors(tcName).toSet())
        {
            Testcase* dependentTestcase = m_testcases.value(dependentTcName);
            QList<Depends*> dependsItems = m_testcaseGraph.edges(tcName, dependentTcName);
            for (const auto& dependsItem: dependsItems)
            {
                QString aliasName = tcName;
                if (!dependsItem->alias().isEmpty())
                {
                    aliasName = dependsItem->alias();
                }
                dependentTestcase->setDependencyData(aliasName, exports.toVariantList());
            }
        }
    }
}

/*
1. We have a dependency tree m_testcaseGraph that specifies generic
   dependencies between testcases (job classes).

2. We have a job table with all job instances and their
   respective tags.

Now we need to find out which job instance depends on which other job.
Therefore, we need to go through all jobs and:

3. Apply its assigned tags to reevaluate bindings in that job.
   That is necessary because Depends item may change content
   based on the tags.

4. Find one or more matching jobs for that dependency and
   set up a dependency relation in the job dependency tree.

There are exceptions for jobs not explicitly speciying tags
in the Depends item:

- a nontagged job depending on a tagged one matches all job instances

- a tagged job depending on a tagged one matches all job
  instances with equal tags.

The term job class means: all jobs with equal name.

*/
void DependencyResolver::completeResolve(const JobTable& jobs)
{
    for (const auto& job: jobs.values())
    {
        m_jobGraph.insertNode(job);
    }

    for (auto& currentJob: jobs.values())
    {
        TagSet ourTags = currentJob.tags();
        m_testcases[currentJob.name()]->setTags(ourTags);

        // All job classes we are depending on
        QSet<QString> dependencyNames = m_testcaseGraph.predecessors(currentJob.name()).toSet();
        for (const auto& dependencyName: dependencyNames)
        {
            // All Depends items pointing to our job class
            QList<Depends*> dependsItems = m_testcaseGraph.edges(dependencyName, currentJob.name());
            for (const auto& dependsItem: dependsItems)
            {
                // Make the Depends item reevaluate its bindings based on the tags set further up
                dependsItem->evaluateTags();

                QList<Job> precedingJobs = jobs.values(dependencyName);
                // When failing, something went wrong in jobmultiplier
                Q_ASSERT(precedingJobs.length() > 0);

                Dependency dependency;
                dependency.setName(dependencyName);
                dependency.setAlias(dependsItem->alias());
                dependency.setTags(dependsItem->tags());

                bool weAreTagged = !ourTags.isEmpty();
                bool dependencyIsTagged = (precedingJobs.length() > 1);
                bool dependencySpecifiesTags = dependsItem->specifiesTags();

                // Default behavior of Depends item, match all preceding jobs
                // no matter whether they are tagged or not
                if (!dependencySpecifiesTags)
                {
                    // Don't care about tags, just match any job with the dependency name.
                    if (!weAreTagged || !dependencyIsTagged)
                    {
                        for (const auto& precedingJob: precedingJobs)
                        {
                            dependency.incrementCount();
                            m_jobGraph.insertEdge(precedingJob, currentJob, dependency);
                        }
                    }
                    // Either we are tagged or the dependency is tagged.
                    // Use our tags and try to match other jobs 1:1.
                    // If our job is not tagged, we will match every dependency job.
                    else
                    {
                        bool matched = false;
                        for (const auto& precedingJob: precedingJobs)
                        {
                            TagSet precedingTags = precedingJob.tags();
                            if (ourTags == precedingTags)
                            {
                                dependency.incrementCount();
                                m_jobGraph.insertEdge(precedingJob, currentJob, dependency);
                                matched = true;
                                break;
                            }
                        }
                        if (matched == false)
                        {
                            auto dumpTagList = [](const QSet<Tag>& tags){
                                QStringList strings;
                                for (const auto& tag: tags)
                                {
                                    strings << QString("%1:%2").arg(tag.label()).arg(tag.value());
                                }
                                return strings.join(' ');
                            };
                            QmlContext context = qst::qmlDefinitionContext(m_testcases[currentJob.name()]);
                            QString message = QString("At %1:%2: Cannot create one-to-one relation between %3 (%4) and %5 because the tags don't match any job.")
                                    .arg(context.file()).arg(context.line())
                                    .arg(currentJob.name())
                                    .arg(dumpTagList(ourTags))
                                    .arg(dependencyName);
                            QST_ERROR_AND_EXIT(message);
                        }
                    }
                }
                // Match tags from Depends item with tags from depend jobs
                else
                {
                    for (const auto& tagSet: dependency.tags())
                    {
                        for (const auto& precedingJob: precedingJobs)
                        {
                            if (precedingJob.tags() == tagSet)
                            {
                                dependency.incrementCount();
                                m_jobGraph.insertEdge(precedingJob, currentJob, dependency);
                            }
                        }
                    }
                }
            }
        }
    }
}


