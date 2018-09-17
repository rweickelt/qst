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
#include "testcase.h"
#include "testcaseattached.h"
#include "console.h"
#include "exports.h"
#include "proxylogger.h"
#include "project.h"
#include "qst.h"
#include "qstitemvisitor.h"

#include <QtDebug>
#include <QtCore/QDir>
#include <QtCore/QEventLoop>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>

#include <private/qv4engine_p.h>
#include <private/qv8engine_p.h>

QPointer<Testcase> Testcase::m_currentTestCase;

Testcase::Testcase(QObject *parent) : Component(parent)
{
    m_state = Uninitialized;
    m_nextState = Uninitialized;
    m_result = Unfinished;
    m_executionTime = 0;
    m_transitionPending = false;
    m_exports = nullptr;
}

void Testcase::callVisitor(QstItemVisitor* visitor)
{
    visitor->visit(this);
}

void Testcase::handleParserEvent(QstItem::ParserEvent event)
{
    Component::handleParserEvent(event);

    if (event == QstItem::ComponentComplete)
    {
        QStringList availableMethods;
        for (int i = 0; i < metaObject()->methodCount(); i++)
        {
            QMetaMethod method = metaObject()->method(i);
            if (method.methodType() == QMetaMethod::Slot)
            {
                availableMethods << method.name();
            }
        }
        if (!availableMethods.contains("run"))
        {
            QmlContext context = qst::qmlDefinitionContext(this);
            QST_ERROR_AND_EXIT(QString("At %1:%2: Test case does not define a 'run()' method.")
                    .arg(context.file()).arg(context.line()));
        }
    }
}

Testcase::Result Testcase::exec()
{
    this->connect(qmlEngine(this), &QQmlEngine::warnings, this, &Testcase::onQmlEngineWarnings);
    qmlEngine(this)->setOutputWarningsToStandardError(false);

    for (m_state = Uninitialized; m_state != Destroyed; m_state = m_nextState)
    {
        m_transitionPending = false;

        switch (m_state)
        {
        case Uninitialized:
            m_nextState = unitializedStateFunction();
            break;
        case InitingTestCase:
            m_nextState = initingTestCaseStateFunction();
            break;
        case InitingTestFunction:
            m_nextState = initingTestFunctionStateFunction();
            break;
        case Running:
            m_nextState = runningStateFunction();
            break;
        case CleaningUpTestFunction:
            m_nextState = cleaningUpTestFunctionStateFunction();
            break;
        case CleaningUpTestCase:
            m_nextState = cleaningUpTestCaseStateFunction();
            break;
        default:
            Q_ASSERT(false); // should never reach this point
            break;
        }
    }

    qmlEngine(this)->setOutputWarningsToStandardError(true);
    this->disconnect(qmlEngine(this), &QQmlEngine::warnings, this, &Testcase::onQmlEngineWarnings);

    return m_result;
}

Testcase::State Testcase::unitializedStateFunction()
{
    m_result = Unfinished;
    m_currentTestCase = this;

    QList<Component*> nestedComponents = findChildren<Component*>(QString(), Qt::FindChildrenRecursively) << this;
    // created() is a signal that even QML children can subscribe.
    for (Component* child : nestedComponents)
    {
        QObject* attached = qmlAttachedPropertiesObject<Testcase>(child, false);
        if (attached != NULL)
        {
            m_attachedObjects << attached;
            QMetaObject::invokeMethod(attached, "created");
        }
    }
    emit created();

    return InitingTestCase;
}


Testcase::State Testcase::initingTestCaseStateFunction()
{
    QList<Component*> nestedComponents = findChildren<Component*>(QString(), Qt::FindChildrenRecursively) << this;
    for (auto child : nestedComponents)
    {
        child->initTestCase();
    }

    return InitingTestFunction;
}

Testcase::State Testcase::initingTestFunctionStateFunction()
{
    QList<Component*> nestedComponents = findChildren<Component*>(QString(), Qt::FindChildrenRecursively) << this;
    for (auto child : nestedComponents)
    {
        child->initTestFunction();
    }

    return Running;
}

Testcase::State Testcase::runningStateFunction()
{
    for (auto attached : m_attachedObjects)
    {
        QMetaObject::invokeMethod(attached, "started");
    }
    emit started();
    if (m_result != Unfinished)
    {
        return CleaningUpTestFunction;
    }

    QCoreApplication::processEvents();

    m_timer.start();

    bool invocationSuccessful = QMetaObject::invokeMethod(this, "run");
    Q_ASSERT(invocationSuccessful == true);

    m_executionTime = m_timer.elapsed();
    m_timer.invalidate();

    return CleaningUpTestFunction;
}

Testcase::State Testcase::cleaningUpTestFunctionStateFunction()
{
    for (auto attached : m_attachedObjects)
    {
        QMetaObject::invokeMethod(attached, "finished");
    }
    emit finished();

    LogInfo info;
    switch (m_result)
    {
    case Unfinished:
    case Success:
        info.type = LogInfo::Success;
        info.test = displayName();
        info.component = name();
        info.file = m_callerFile;
        ProxyLogger::instance()->print(info);
        break;
    case Fail:
        info.type = LogInfo::Fail;
        info.test = displayName();
        info.component = name();
        info.file = m_callerFile;
        info.line = m_callerLine;
        info.message = m_message;
        ProxyLogger::instance()->print(info);
        break;
    }

    QList<Component*> nestedComponents = findChildren<Component*>(QString(), Qt::FindChildrenRecursively) << this;
    for (auto child : nestedComponents)
    {
        child->cleanupTestFunction();
    }

    return CleaningUpTestCase;
}

Testcase::State Testcase::cleaningUpTestCaseStateFunction()
{
    // If nothing else has determined the result until now,
    // then it must be successful.
    if (m_result == Unfinished)
    {
        m_result = Success;
    }

    QList<Component*> nestedComponents = findChildren<Component*>(QString(), Qt::FindChildrenRecursively) << this;
    for (auto child : nestedComponents)
    {
        child->cleanupTestCase();
    }

    for (auto attached : m_attachedObjects)
    {
        QMetaObject::invokeMethod(attached, "destruction");
    }
    emit destruction();
    m_currentTestCase.clear();
    return Destroyed;
}


/*
Aborts the test case and sets the test case result to result.

This function is to be called from C++ code.
*/
void Testcase::finishAndExit(Testcase::Result result, const QString& file, int line, const QString& message)
{

    // This seems to be the only way to inject an exception in the Qml engine from C++ context.
    QV4::ExecutionEngine* engine = QV8Engine::getV4(qmlEngine(this)->handle());

    m_result = result;
    m_callerFile = file;
    m_callerLine = line;
    m_message = message;

    switch (result)
    {
    case Testcase::Fail:
        engine->throwError(QString("Testcase::Fail"));
        break;
        // More results, i.e. Skip?
    default:
        // Should never be called with result = Success or anything else.
        Q_ASSERT_X(false, "Testcase::finishAndExit()", message.toLocal8Bit());
        break;
    }
}

void Testcase::waitUntilExpression(QJSValue expression, int milliseconds, const QString& file, int line)
{
    m_callerFile = file;
    m_callerLine = line;

    Q_ASSERT(expression.isCallable());

    QTimer timer;
    timer.setSingleShot(true);
    timer.start(milliseconds);
    for (;;)
    {
        QJSValue result = expression.call();
        if (result.isBool())
        {
            if (result.toBool() == true)
            {
                return;
            }
            else if (!timer.isActive())
            {
                Testcase::instance()->finishAndExit(Testcase::Fail,  __FILE__, __LINE__, "timeout");
                return;
            }
        }
        else if (result.isError())
        {
            // Catch potential exceptions in the expression or
            // exceptions that have been thrown by finishAndExit()
            // while evaluating the expression.
            QV4::ExecutionEngine* engine = QV8Engine::getV4(qmlEngine(this)->handle());
            // The error object is an error string that starts with "Error: "
            // followed by a message. We re-use the message, but remove the first part.
            engine->throwError(result.toString().remove(QRegExp("^Error\\:\\s")));
            return;
        }
        else
        {
            qDebug() << "Type " << result.isBool();
            // result has an unexpected type. But this should not happen.
            Q_ASSERT(false);
        }

        QCoreApplication::processEvents(QEventLoop::AllEvents, milliseconds);
    }
}

Exports* Testcase::exportsItem() const
{
    QList<Exports*> result = findChildren<Exports*>(QString(), Qt::FindDirectChildrenOnly);
    Q_ASSERT(result.length() <= 1);
    return result.value(0, nullptr);
}

qint64 Testcase::elapsedTime() const
{
    if (m_timer.isValid())
    {
        // Timer is only valid while run()
        return m_timer.elapsed();
    }
    else
    {
        // Otherwise the result will be 0 or the execution time.
        return m_executionTime;
    }
}


void Testcase::waitMilliseconds(int milliseconds, const QString& file, int line)
{
    m_callerFile = file;
    m_callerLine = line;
    QEventLoop eventLoop;
    QTimer timer;
    timer.setSingleShot(true);
    timer.setTimerType(Qt::PreciseTimer);
    timer.connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
    timer.start(milliseconds);
    eventLoop.exec();
}

/*
This slot is invoked whenever an exception in the QML engine observes
an exception.
*/
void Testcase::onQmlEngineWarnings(const QList<QQmlError> &warnings)
{
    Q_ASSERT(warnings.size() == 1);

    QV4::ExecutionEngine* engine = QV8Engine::getV4(qmlEngine(this)->handle());
    // qDebug() << "onQmlEngineWarnings Calling depth: " << engine->callDepth;

    const QQmlError& error = warnings.first();

    if (error.description() != "Error: Testcase::Fail")
    {
        // Exception caused by QML code. This includes syntax and reference errors.
        // We need to store heritage information here.
        m_result = Fail;
        m_callerFile = error.url().toString();
        m_callerLine = error.line();
        m_message = error.description();
    }

    // Inject exceptions as long as we are somewhere on the call stack.
    // This is necessary when C++ functions are sit between multiple
    // QML functions.
    if (engine->callDepth > 0)
    {
        engine->throwError("Testcase::Fail");
    }
}

TestcaseAttached* Testcase::qmlAttachedProperties(QObject* object)
{
    return new TestcaseAttached(object);
}

Project* Testcase::project() const
{
    Project* project = qvariant_cast<Project*>(qmlContext(this)->contextProperty("project"));
    Q_ASSERT(project != NULL);
    return project;
}

Testcase* Testcase::instance()
{
    return m_currentTestCase.data();
}

void Testcase::setDisplayName(const QString& name)
{
    m_displayName = name;
}

QString Testcase::displayName() const
{
    if (!m_displayName.isEmpty())
    {
        return m_displayName;
    }
    else
    {
        return name();
    }
}

void Testcase::setWorkingDirectory(const QString& path)
{
    if (m_workingDirectory != path)
    {
        m_workingDirectory = path;
        emit workingDirectoryChanged();
    }
}

// Work-around for https://bugreports.qt.io/browse/QTBUG-69075
// Instead of making the Exports item's content available under 'name' directly,
// we use an intermediate 'dependencies' property.
void Testcase::attachDependencyExport(const QString& name, const QVariant& values)
{
    if (m_dependencies.contains(name))
    {
        m_dependencies.clear(name);
    }
    m_dependencies.insert(name, values);
}
