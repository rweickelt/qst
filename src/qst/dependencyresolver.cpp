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

#include <QtCore/QVariantMap>
#include <QtCore/QSet>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlProperty>

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
    Q_ASSERT(!item->name().isEmpty());

//    m_resolver.m_dependencies.insert(m_currentTestcase, item);
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

    // Check for Depends items pointing to non-existing testcases.
    for (const auto& dependsItem: m_testcaseGraph.edges())
    {
        if (!tcNames.contains(dependsItem->name()))
        {
            QmlContext context = qst::qmlDefinitionContext(dependsItem);
            QString message = QString("At %1:%2: Name '%3' is not an existing testcase.")
                    .arg(context.file()).arg(context.line())
                    .arg(dependsItem->name());
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

        QVariantMap exportMap = exportItem->toVariantMap();
        for (const auto& dependentTcName: m_testcaseGraph.successors(tcName))
        {
            Testcase* dependentTestcase = m_testcases.value(dependentTcName);
            Depends* dependsItem = m_testcaseGraph.edge(tcName, dependentTcName);
            QString aliasName = tcName;
            if (!dependsItem->alias().isEmpty())
            {
                aliasName = dependsItem->alias();
            }
//            qDebug() << "Attaching " << dependsItem->name() << " as " << aliasName << " to " << dependentTcName;
            QQmlContext* context = qmlContext(dependentTestcase)->parentContext();
            context->setContextProperty(aliasName.toLatin1(), exportMap);
        }
    }
}

void DependencyResolver::completeResolve(const JobLookupTable& jobs)
{
    for (const auto& job: jobs.values())
    {
        m_jobGraph.insertNode(job);
    }

    for (auto& currentJob: jobs.values())
    {
        TagSet ourTags = currentJob.tags();
        QList<Testcase> dependencies;
        for (const auto& dependencyName: m_testcaseGraph.predecessors(currentJob.testcase()->name()))
        {
            Depends* dependsItem = m_testcaseGraph.edge(dependencyName, currentJob.testcase()->name());
            Dependency dependency;
            dependency.setName(dependencyName);
            dependency.setAlias(dependsItem->alias());
            dependency.setTags(dependsItem->tags());

            JobList precedingJobs = jobs.values(dependencyName);
            // When failing, something went wrong in jobmultiplier
            Q_ASSERT(precedingJobs.length() > 0);

            bool weAreTagged = !ourTags.isEmpty();
            bool dependencyIsTagged = (precedingJobs.length() > 1);
            bool dependsItemSpecifiesTags = !dependency.tags().isEmpty();

            if (!dependsItemSpecifiesTags)
            {
                // Default behavior of Depends item
                if (!weAreTagged || !dependencyIsTagged)
                {
                    for (const auto& precedingJob: precedingJobs)
                    {
                        m_jobGraph.insertEdge(precedingJob, currentJob, dependency);
                    }
                }
                else
                {
                    // Use our tags and try to match other jobs 1:1
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
                                strings << tag.toString();
                            }
                            return strings.join(' ');
                        };
                        QmlContext context = qst::qmlDefinitionContext(currentJob.testcase());
                        QString message = QString("At %1:%2: Cannot create one-to-one relation between %3 (%4) and %5 because the tags don't match any job.")
                                .arg(context.file()).arg(context.line())
                                .arg(currentJob.testcase()->name())
                                .arg(dumpTagList(ourTags))
                                .arg(dependencyName);
                        QST_ERROR_AND_EXIT(message);
                    }
                }
            }
            else
            {
                // Match tags from Depends item with tags from depend jobs
                for (const auto& tagSet: dependency.tags())
                {
                    for (const auto& precedingJob: precedingJobs)
                    {
                        if (precedingJob.tags().matches(tagSet))
                        {
                            m_jobGraph.insertEdge(precedingJob, currentJob, dependency);
                        }
                    }
                }
            }
        }
    }
}


