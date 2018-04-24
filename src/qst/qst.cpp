/****************************************************************************
 **
 ** Copyright (C) 2017, 2018 The Qst project.
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
#include "console.h"
#include "proxylogger.h"
#include "qst.h"
#include "testcase.h"

#include <QtCore/QDebug>
#include <QtCore/QHash>
#include <QtCore/QTemporaryFile>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlExpression>
#include <QtQml/QQmlInfo>

#include <private/qqmlcontext_p.h>
#include <private/qqmldata_p.h>
#include <private/qv4engine_p.h>
#include <private/qv8engine_p.h>

namespace {
    QHash<QQmlEngine*, QstService*> instances;

    QString singleInstanceQmlQuery = "import QtQml 2.0\n"
                                     "import qst 1.0\n"
                                     "\n"
                                     "QtObject { property var instance : Qst; }\n";
}

namespace qst {

void error(const QString& message)
{
    Console::printError(QString("Error: %1").arg(message));
    ::exit(qst::ExitApplicationError);
}

void info(const QString& message, const QString& file, int line)
{
    LogInfo info {
        .test = Testcase::instance()->displayName(),
        .component = "",
        .file = file,
        .line = line,
        .message = message,
        .type = LogInfo::Info
    };
    if (file.isEmpty())
    {
        QmlContext context = QstService::instance(qmlEngine(Testcase::instance()))->qmlCallerContext();
        info.file = context.file();
        info.line = context.line();
    }
    ProxyLogger::instance()->print(info);
}

QmlContext qmlDefinitionContext(const QObject* object)
{
    QmlContext result;

    if (object)
    {
        QQmlData* data = QQmlData::get(object);
        if (data && data->outerContext)
        {
            result["file"] = data->outerContext->url().path();
            result["line"] = data->lineNumber;
            result["column"] = data->columnNumber;
        }
    }

    return result;
}

void verify(bool condition, const QString& message, const QString& file, int line)
{
    if (!condition)
    {
        if ((file.isEmpty() && line == 0))
        {
            QmlContext context = QstService::instance(qmlEngine(Testcase::instance()))->qmlCallerContext();
            Testcase::instance()->finishAndExit(Testcase::Fail, context.file(), context.line(), message);
        }
        else
        {
            Testcase::instance()->finishAndExit(Testcase::Fail, file, line, message);
        }
    }
}

void warning(const QString& message, const QString& file, int line)
{
    LogInfo info {
        .test = Testcase::instance()->displayName(),
        .component = "",
        .file = file,
        .line = line,
        .message = message,
        .type = LogInfo::Warning
    };
    if (file.isEmpty())
    {
        QmlContext context = QstService::instance(qmlEngine(Testcase::instance()))->qmlCallerContext();
        info.file = context.file();
        info.line = context.line();
    }
    ProxyLogger::instance()->print(info);
}

}

QstService::QstService(QObject *parent) : QObject(parent)
{
}

Testcase* QstService::currentTestcase()
{
    return Testcase::instance();
}


QString QstService::hostOS() const
{
    QString kernel = QSysInfo::kernelType();
    if (kernel == "winnt")
    {
        return "windows";
    }
    else if (kernel == "darwin")
    {
        return QSysInfo::productType();
    }
    else
    {
        return kernel;
    }
}


void QstService::error(const QString& message, QString file, int line)
{
    if (file.isEmpty())
    {
        QmlContext context = qmlCallerContext();
        file = context.file();
        line = context.line();
    }
    Console::printError(QString("Error: %1 in %2:%3").arg(message).arg(file).arg(line));
    ::exit(qst::ExitApplicationError);
}


QmlContext QstService::qmlCallerContext()
{
    QQmlEngine* engine = qmlEngine(this);
    Q_ASSERT(engine != nullptr);

    QV4::ExecutionEngine* executionEngine = QV8Engine::getV4(engine->handle());
    Q_ASSERT(executionEngine != nullptr);

    QV4::StackTrace trace = executionEngine->stackTrace(2);
    QV4::StackFrame& frame = trace.last();
    QmlContext result;
    result["file"] = QUrl(frame.source).path();
    result["line"] = frame.line;
    result["column"] = frame.column;

    return result;
}

QVariantList QstService::qmlCallerTrace()
{
    QQmlEngine* engine = qmlEngine(this);
    Q_ASSERT(engine != nullptr);

    QV4::ExecutionEngine* executionEngine = QV8Engine::getV4(engine->handle());
    Q_ASSERT(executionEngine != nullptr);

    QVariantList result;
    QV4::StackTrace trace = executionEngine->stackTrace();
    for (int i = 0; i < trace.length(); i++)
    {
        const QV4::StackFrame& frame = trace.at(i);
        QmlContext context;
        context.insert("file", QUrl(frame.source).path());
        context.insert("line", frame.line);
        context.insert("column", frame.column);
        result.append(context);
    }
    return result;
}

QmlContext QstService::qmlDefinitionContext(QObject* object)
{
    return qst::qmlDefinitionContext(object);
}


QstService* QstService::instance(QQmlEngine* engine)
{
    Q_ASSERT(engine != nullptr);

    // Singleton objects that were created in QML are tricky.
    // I couldn't find another way to look the instance up.
    // See https://stackoverflow.com/questions/48243936/how-to-find-out-the-qqmlengine-instance-of-a-singleton-object
    //
    if (!instances.contains(engine))
    {
        QTemporaryFile tempFile;
        tempFile.open();
        tempFile.write(singleInstanceQmlQuery.toLatin1());
        tempFile.close();
        QQmlComponent component(engine, QUrl::fromLocalFile(tempFile.fileName()));
        QObject* item = component.create(engine->rootContext());
        QstService* service = qvariant_cast<QstService*>(item->property("instance"));
        instances.insert(engine, service);
    }

    return instances.value(engine); // only until we allow multiple engines
}

void QstService::info(const QVariant &message)
{
    qst::info(message.toString(), "", 0);
}
