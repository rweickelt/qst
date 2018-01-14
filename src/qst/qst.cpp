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
        .test = Testcase::instance()->name(),
        .component = "",
        .file = file,
        .line = line,
        .message = message,
        .type = LogInfo::Info
    };
    if ((file.isEmpty() && line == 0))
    {
        info.file = QstService::instance(qmlEngine(Testcase::instance()))->qmlCallerFile();
        info.line = QstService::instance(qmlEngine(Testcase::instance()))->qmlCallerLine();
    }
    ProxyLogger::instance()->print(info);
}

void verify(bool condition, const QString& message, const QString& file, int line)
{
    if (!condition)
    {
        if ((file.isEmpty() && line == 0))
        {
            Testcase::instance()->finishAndExit(Testcase::Fail,
                    QstService::instance(qmlEngine(Testcase::instance()))->qmlCallerFile(),
                    QstService::instance(qmlEngine(Testcase::instance()))->qmlCallerLine(),
                    message);
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
        .test = Testcase::instance()->name(),
        .component = "",
        .file = file,
        .line = line,
        .message = message,
        .type = LogInfo::Warning
    };
    if ((file.isEmpty() && line == 0))
    {
        info.file = QstService::instance(qmlEngine(Testcase::instance()))->qmlCallerFile();
        info.line = QstService::instance(qmlEngine(Testcase::instance()))->qmlCallerLine();
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
        file = qmlCallerFile();
        line = qmlCallerLine();
    }
    Console::printError(QString("Error: %1 in %2:%3").arg(message).arg(file).arg(line));
    ::exit(qst::ExitApplicationError);
}


QString QstService::qmlCallerFile()
{
    QQmlEngine* engine = QQmlEngine::contextForObject(this)->engine();
    Q_ASSERT(engine != nullptr);

    QV4::ExecutionEngine* executionEngine = QV8Engine::getV4(engine->handle());
    QV4::StackTrace trace = executionEngine->stackTrace(2);
    QV4::StackFrame& frame = trace.last();
    return frame.source;
}


int QstService::qmlCallerLine()
{
    QQmlEngine* engine = QQmlEngine::contextForObject(this)->engine();
    Q_ASSERT(engine != nullptr);

    QV4::ExecutionEngine* executionEngine = QV8Engine::getV4(engine->handle());
    QV4::StackTrace trace = executionEngine->stackTrace(2);
    QV4::StackFrame& frame = trace.last();
    return frame.line;
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
