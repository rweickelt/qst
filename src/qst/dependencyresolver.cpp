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
#include "resourceitem.h"
#include "testcase.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QVariantMap>
#include <QtCore/QSet>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>

namespace {
    QRegularExpression jsIdentifierPattern = QRegularExpression("^[A-Za-z][\\w]*$");
}

class ItemGatherVisitor: public QstItemVisitor
{
public:
    ItemGatherVisitor(DependencyResolver&);

protected:
    virtual void visit(ResourceItem* item) final;
    virtual void visit(Testcase* item) final;

    DependencyResolver& m_resolver;
};

class DependencyVisitor : public QstItemVisitor
{
public:
    DependencyVisitor(DependencyResolver&);

protected:
    virtual void visit(Depends* item) final;
    virtual void visit(Exports* item) final;
    virtual void visit(Testcase* item) final;

    DependencyResolver& m_resolver;
    Testcase* m_currentTestcase = nullptr;
};

ItemGatherVisitor::ItemGatherVisitor(DependencyResolver& resolver)
    : m_resolver(resolver)
{
}

void ItemGatherVisitor::visit(ResourceItem* item)
{
    m_resolver.m_resources.insert(item->name(), item);
    m_resolver.m_resourceGraph.insertNode(item->name());
}

void ItemGatherVisitor::visit(Testcase* item)
{
    m_resolver.m_testcases.insert(item->name(), item);
    m_resolver.m_testcaseGraph.insertNode(item->name());
}

DependencyVisitor::DependencyVisitor(DependencyResolver& resolver)
    : m_resolver(resolver)
{
}

void DependencyVisitor::visit(Depends* item)
{
    if (m_resolver.m_testcases.contains(item->name()))
    {
        m_resolver.m_testcaseGraph.insertEdge(item->name(), m_currentTestcase->name(), item);
    }
    else if (m_resolver.m_resources.contains(item->name()))
    {
        m_resolver.m_resourceGraph.insertEdge(item->name(), m_currentTestcase->name(), item);
    }
    else
    {
        QmlContext context = qst::qmlDefinitionContext(item);
        QString message = QString("%1:%2: The property name '%3' is neither an existing testcase nor resource.")
                .arg(context.file()).arg(context.line())
                .arg(item->name());
        m_resolver.m_errors << message;
    }

    if (item->name().isEmpty())
    {
        QmlContext context = qst::qmlDefinitionContext(item);
        QString message = QString("%1:%2: The name must not be empty.")
                .arg(context.file()).arg(context.line());
        m_resolver.m_errors << message;
        return;
    }
    else if ((!item->alias().isEmpty()) && (!jsIdentifierPattern.match(item->alias()).hasMatch()))
    {
        QmlContext context = qst::qmlDefinitionContext(item);
        QString message = QString("%2:%3: The alias '%1' must be a valid JavaScript identifier.")
                .arg(item->alias())
                .arg(context.file()).arg(context.line());
        m_resolver.m_errors << message;
        return;
    }
    else if ((item->alias().isEmpty()) && (!jsIdentifierPattern.match(item->name()).hasMatch()))
    {
        QmlContext context = qst::qmlDefinitionContext(item);
        QString message = QString("%2:%3: The name '%1' must be a valid JavaScript identifier when no alias is defined.")
                .arg(item->name())
                .arg(context.file()).arg(context.line());
        m_resolver.m_errors << message;
        return;
    }
}

void DependencyVisitor::visit(Exports* item)
{
    m_resolver.m_exports.insert(m_currentTestcase->name(), item);
}

void DependencyVisitor::visit(Testcase* item)
{
    m_currentTestcase = item;
}

DependencyResolver::DependencyResolver(ProjectDatabase* db): m_db(db)
{
}

void DependencyResolver::beginResolve(const QList<QstDocument*> &documents)
{
    if (documents.isEmpty())
    {
        return;
    }

    ItemGatherVisitor v1(*this);
    for (auto& document: documents)
    {
        document->object->accept(&v1);
    }

    DependencyVisitor v2(*this);
    for (auto& document: documents)
    {
        document->object->accept(&v2);
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
}

/*
1. We have a dependency tree m_testcaseGraph that specifies generic
   dependencies between testcases (job classes).

2. We have a depndency tree m_resourceGraph that specifies dependencies
   between resources and testcases (job classes).

2. We have a job table with all job instances and their
   respective tags.

Now we need to find out which job instance depends on which other job
and whether the amount of resource instances per dependency can be
satisfied. Therefore, we need to go through all jobs and:

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
void DependencyResolver::completeResolve(const JobTable& jobs, const ResourceTable& resources)
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
                Q_ASSERT(dependsItem->name() == dependencyName);

                // Make the Depends item reevaluate its bindings based on the tags set further up
                dependsItem->evaluateTags();

                QList<Job> precedingJobs = jobs.values(dependencyName);
                // When failing, something went wrong in jobmultiplier
                Q_ASSERT(precedingJobs.length() > 0);

                Dependency dependency;
                dependency.setName(dependencyName);
                dependency.setAlias(dependsItem->alias());
                dependency.setTags(dependsItem->tags());
                dependency.setType(Dependency::Type::Job);

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
                            m_errors << message;
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
                                m_jobGraph.insertEdge(precedingJob, currentJob, dependency);
                            }
                        }
                    }
                }
            }
        }

        //
        // Get all resource names we are depending on. The job might define multiple
        // Depends items and hence, has multiple edges in the resource graph.
        //
        dependencyNames = m_resourceGraph.predecessors(currentJob.name()).toSet();
        for (const auto& dependencyName: dependencyNames)
        {
            //
            // There might be multiple Depends items with the same name. Get all with
            // the current dependency name.
            //
            QList<Depends*> dependsItems = m_resourceGraph.edges(dependencyName, currentJob.name());
            for (const auto& dependsItem: dependsItems)
            {
                Q_ASSERT(dependsItem->name() == dependencyName);

                //
                // Make the Depends item reevaluate its bindings based on the tags set further up
                //
                dependsItem->evaluateTags();

                QSet<Resource> potentialResources = resources.values(dependencyName).toSet();
                QSet<Resource> filteredResources;
                Q_ASSERT(potentialResources.size() > 0);

                Dependency dependency;
                dependency.setCount(dependsItem->count());
                dependency.setName(dependencyName);
                dependency.setAlias(dependsItem->alias());
                dependency.setTags(dependsItem->tags());
                dependency.setType(Dependency::Type::Resource);

//                qDebug() << "Dependency: " << dependsItem->name() << currentJob.name();

                if (!dependsItem->specifiesTags())
                {
                    //
                    // If the dependency does not specify any tags, we include any resource from
                    // the (named) pool. The resource limit is determined later.
                    //
                    filteredResources = potentialResources;
                }
                else
                {
                    //
                    // Match each tagset of the Depends item with resources in the (named) pool. We might get
                    // more resources than needed, but that will be handled in the execution stage.
                    //
                    for (const auto& tagSet: dependency.tags())
                    {
                        for (const auto& potentialResource: potentialResources)
                        {
                            //
                            // Todo: Do not only match against tags of the resource
                            //       but take any property into account.
                            //
                            if (potentialResource.tags().contains(tagSet))
                            {
                                filteredResources << potentialResource;

                            }
                        }
                    }
                }
                //
                // Ensure that there are sufficient resources available.
                //
                if (filteredResources.size() < dependency.count())
                {
                    QmlContext context = qst::qmlDefinitionContext(dependsItem);
                    QString message = QString("At %1:%2: %3 resources of name '%4' required, but only (%5) available")
                            .arg(context.file()).arg(context.line())
                            .arg(dependency.count())
                            .arg(dependency.name())
                            .arg(filteredResources.size());
                    m_errors << message;
                }
                m_resourcesPerJob.insert(currentJob, { dependency, potentialResources });
            }
        }

    }
}

