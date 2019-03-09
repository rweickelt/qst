/****************************************************************************
 **
 ** Copyright (C) 2017-2019 The Qst project.
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

#include "applicationoptions.h"
#include "component.h"
#include "exports.h"
#include "jobdispatcher.h"
#include "projectdatabase.h"
#include "tag.h"
#include "testcase.h"
#include "textfile.h"
#include "qst.h"

#include <QtCore/QFileInfo>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlProperty>

JobDispatcher::JobDispatcher(const ProjectDatabase& database)
    : m_db(database)

{
    m_engine.setParent(this);
    m_projectWorkingDirectory.setPath(m_db.project["workingDirectory"].toString());

    for (const auto& path : ApplicationOptions::instance()->importPaths)
    {
        m_engine.addImportPath(path);
    }
    TextFile::registerJSType(&m_engine);
}

JobDispatcher::~JobDispatcher()
{
}

#include <QtDebug>

void JobDispatcher::dispatch(Job job)
{
    QString name = job.name();
    QString displayName = name;
    QString workingDirectoryName = name;
    QList<Tag> tags = job.tags().toList();
    std::sort(tags.begin(), tags.end());

    m_engine.rootContext()->setContextProperty("profile", m_db.profile);
    m_engine.rootContext()->setContextProperty("project", m_db.project);
    m_engine.rootContext()->setContextProperty("path", QFileInfo(job.filePath()).dir().absolutePath());

    Component::resetInstancesCounter();
    QQmlComponent component(&m_engine, job.filePath());
    QstItem* rootItem = qobject_cast<QstItem*>(component.beginCreate(m_engine.rootContext()));
    if (component.errors().length() > 0)
    {
        QST_ERROR_AND_EXIT(QString("Error while loading '%1': %2").arg(job.filePath()).arg(component.errorString()));
    }

    rootItem->afterClassBegin();
    for (auto& item: rootItem->findChildren<QstItem*>())
    {
        item->afterClassBegin();
    }

    Testcase* testcase;
    if (rootItem->objectName() == name)
    {
        testcase = qobject_cast<Testcase*>(rootItem);
        Q_ASSERT(testcase != nullptr);
    }
    else
    {
        testcase = rootItem->findChild<Testcase*>(name);
        if (testcase == nullptr)
        {
            QST_ERROR_AND_EXIT(QString("Error while loading '%1': Item '%2' not found").arg(job.filePath()).arg(name));
        }
    }

    m_engine.rootContext()->setContextProperty("test", testcase);

    if (job.tags().size() > 0)
    {
        QStringList tagsStrings;
        for (const auto& tag: tags)
        {
            tagsStrings << tag.value();

            QQmlProperty property(testcase, tag.label());
            Q_ASSERT(property.isProperty());
            Q_ASSERT(property.isWritable());
            property.write(tag.value());
        }

        displayName = QString("%1 %2 [ %3 ]")
                .arg(name)
                .arg(job.id(), 7, 36, QChar('0'))
                .arg(tagsStrings.join(" "));

        workingDirectoryName = QString("%1-%2")
                .arg(name)
                .arg(job.id(), 7, 36, QChar('0'));
    }

    QString workingDirectory = createTestcaseWorkingDirectory(workingDirectoryName);
    testcase->setWorkingDirectory(workingDirectory);
    testcase->setDisplayName(displayName);

    testcase->setDependencyData(job.dependenciesData());

    component.completeCreate();
    if (component.errors().length() > 0)
    {
        QST_ERROR_AND_EXIT(QString("Error while completing creation of '%1': %2").arg(job.filePath()).arg(component.errorString()));
    }
    rootItem->afterComponentComplete();

    Testcase::Result result = testcase->exec();

    job.setResult(result);
    if (Exports* exports = testcase->exportsItem())
    {
        job.setExports(parseExports(exports));
    }

    emit finished(job);
}

QString JobDispatcher::createTestcaseWorkingDirectory(const QString& name)
{
    QString workDirPath = m_projectWorkingDirectory.absolutePath();
    if (!QFileInfo::exists(workDirPath))
    {
        if (!QDir().mkpath(workDirPath))
        {
            QST_ERROR_AND_EXIT(QString("Could not create working directory '%1'.").arg(workDirPath));
        }
    }
    else if (!QFileInfo(workDirPath).isDir())
    {
        QST_ERROR_AND_EXIT(QString("Working directory path '%1' is not a valid directory.").arg(workDirPath));
    }
    else
    {
        if (!QFileInfo(workDirPath).isWritable())
        {
            QST_ERROR_AND_EXIT(QString("Working directory '%1' is not writable.").arg(workDirPath));
        }
    }

    QDir testcaseWorkDir(m_projectWorkingDirectory.absoluteFilePath(name));

    if (testcaseWorkDir.exists())
    {
        if (!testcaseWorkDir.removeRecursively())
        {
            QST_ERROR_AND_EXIT(QString("Could not wipe directory '%1'.")
                                .arg(testcaseWorkDir.absolutePath()));
        }
    }

    if ((!m_projectWorkingDirectory.mkdir(name)))
    {
        QST_ERROR_AND_EXIT(QString("Could not create working directory '%1'.")
                            .arg(testcaseWorkDir.absolutePath()));
    }

    return testcaseWorkDir.absolutePath();
}

QVariantMap JobDispatcher::parseExports(Exports* item)
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
