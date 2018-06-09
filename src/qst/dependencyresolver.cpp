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
#include <QtCore/QVector>
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

    Dependency dependency;

//    m_resolver.m_dependencies.insert(m_currentTestcase, item);
    m_resolver.m_testcaseGraph.insertEdge(item->name(), m_currentTestcase->name(), item);
}

void DependencyVisitor::visit(Exports* item)
{
    Q_ASSERT(m_currentTestcase->exportsItem() == nullptr);
    m_resolver.m_exports.insert(m_currentTestcase, item);
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

    // Attach preliminary exports item to dependents
    for (auto& node: m_testcaseGraph.nodes())
    {
        Testcase* testcase = m_testcases[node];
        if (Exports* exportItem = testcase->exportsItem())
        {
            QVariantMap exportMap = exportItem->toVariantMap();
            for (auto& dependent: m_testcaseGraph.successors(node))
            {
                Testcase* dependentTestcase = m_testcases[dependent];
//                Depends* dependsItem = m_testcaseGraph.edge(dependent, node);
                QString targetName = dependent; // todo: when enabling "as" property, the code above must be enabled (and fixed).
                // Not implemented yet
                //                for (const auto& dependency: dependent->dependencies)
                //                {
                //                    if ((dependency->on() == node->testcaseItem->name())
                //                            && (!dependency->as().isEmpty()))
                //                    {
                //                        targetName = dependent->targetNames.at(index);
                //                    }
                //                }
                QQmlContext* context = qmlContext(dependentTestcase)->parentContext();
                context->setContextProperty(targetName.toLatin1(), exportMap);
            }
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
        QSet<Tag> ourTags = currentJob.tags().toSet();
        QList<Testcase> dependencies;
        for (const auto& dependencyName: m_testcaseGraph.predecessors(currentJob.testcase()->name()))
        {
            JobList precedingJobs = jobs.values(dependencyName);
            // When failing, something went wrong in jobmultiplier
            Q_ASSERT(precedingJobs.length() > 0);

            // If the dependency is untagged and we are tagged or when
            // we are untagged, create a n:1/1:1 relation.
            if ((precedingJobs.length() == 1) || (ourTags.isEmpty()))
            {
                for (const auto& precedingJob: precedingJobs)
                {
                    m_jobGraph.insertEdge(precedingJob, currentJob);
                }
            }
            // Otherwise try to match by tags
            else
            {
                bool matched = false;
                for (const auto& precedingJob: precedingJobs)
                {
                    QSet<Tag> precedingTags = precedingJob.tags().toSet();
                    if (ourTags == precedingTags)
                    {
                        m_jobGraph.insertEdge(precedingJob, currentJob);
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
    }
}


