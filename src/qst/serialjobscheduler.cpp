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

#include "depends.h"
#include "exports.h"
#include "serialjobscheduler.h"
#include "qstdocument.h"
#include "qstitemvisitor.h"
#include "testcase.h"

#include <QtQml/QQmlContext>
#include <QtDebug>

namespace {
    QStringList readylistToString(QList<Job> jobs)
    {
        QStringList names;
        for (const auto& job: jobs)
        {
            names << job.testcase()->name();
        }
        return names;
    }
}


SerialJobScheduler::SerialJobScheduler(const JobTable& jobs,
                                       const DependencyGraph& dependencies,
                                       QObject* parent)
    : QObject(parent)
{
    m_dependencies = dependencies;
    m_todo = jobs;

    for (const auto& node: m_dependencies.nodes())
    {
        m_remainingDependencies.insert(node->testcaseItem->name(), node->dependencies.toSet());
    }
}

void SerialJobScheduler::onJobFinished(const Job& job)
{
    QVariantMap exportedValues;

    QString name = job.testcase()->name();
    m_done.insert(name, job);

    DependencyNode node = m_dependencies.node(name);
    if (node->exportItem)
    {
        exportedValues = parseExports(node->exportItem);
    }

    QStringList dependents = m_dependencies.children(name);
    for (const auto& dependent: dependents)
    {
        // Attach exported values
        if (node->exportItem)
        {
            QQmlContext* context = qmlContext(node->testcaseItem)->parentContext();
            Q_ASSERT(context);
            context->setContextProperty(name.toLatin1().constData(), exportedValues);
        }

        m_remainingDependencies[dependent].remove(name);
        if (m_remainingDependencies[dependent].isEmpty())
        {
            m_readyList << m_todo.values(dependent);
            m_todo.remove(dependent);
        }
    }

    if (!m_readyList.isEmpty())
    {
        emit jobReady(m_readyList.takeFirst());
        return;
    }
    else
    {
        emit finished();
    }
}

void SerialJobScheduler::start()
{
    if (m_dependencies.isEmpty())
    {
        emit finished();
        return;
    }

    QStringList names = m_dependencies.children(QString());

    for (const auto& name: names)
    {
        m_readyList << m_todo.values(name);
        m_todo.remove(name);
    }
    Q_ASSERT(!m_readyList.isEmpty());

    emit jobReady(m_readyList.takeFirst());
}


QVariantMap SerialJobScheduler::parseExports(Exports* item)
{
    const static QStringList ignoredProperties{ "objectName", "nestedComponents" };
    QVariantMap result;
    const QMetaObject *metaobject = item->metaObject();
    int count = metaobject->propertyCount();
    for (int i=0; i<count; ++i) {
        QMetaProperty metaproperty = metaobject->property(i);
        const char *name = metaproperty.name();

        if (ignoredProperties.contains(QLatin1String(name)) || (!metaproperty.isReadable()))
        {
            continue;
        }

        QVariant value = item->property(name);
        result[QLatin1String(name)] = value;
    }
    return result;
}

