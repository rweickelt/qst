/****************************************************************************
 **
 ** Copyright (C) 2017 The Qst project.
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
#include "log.h"
#include "project.h"
#include "qst.h"

#include <QtDebug>
#include <QtCore/QDir>
#include <QtCore/QEventLoop>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>

#include <private/qv4engine_p.h>
#include <private/qv8engine_p.h>

QPointer<TestCase> TestCase::m_currentTestCase;

TestCase::TestCase(QObject *parent) : Component(parent)
{
    m_state = Uninitialized;
    m_nextState = Uninitialized;
    m_result = Unfinished;
    m_executionTime = 0;
    m_transitionPending = false;
}

void TestCase::classBegin()
{
}

void TestCase::componentComplete()
{
    QDir projectWorkDir(project()->workingDirectory());
    if (!projectWorkDir.exists(m_name))
    {
        if ((!projectWorkDir.mkdir(m_name)))
        {
            Console::printError(QString("Could not create working directory '%1'.")
                                .arg(projectWorkDir.absoluteFilePath(m_name)));
            ::exit(qst::ExitApplicationError);
        }
    }
}

TestCase::Result TestCase::exec()
{
    QList<Component*> children = childrenByType<Component>();

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

        QMetaEnum metaEnum = QMetaEnum::fromType<TestCase::State>();
        QString state = metaEnum.valueToKey(m_state);
        QString nextState = metaEnum.valueToKey(m_nextState);
        log()->printInfo("TestCase", QString("Transitioning from state %1 to %2.").arg(state).arg(nextState));
    }

    return m_result;
}

TestCase::State TestCase::unitializedStateFunction()
{
//    Console::printToStdOut(QString("# Starting %1").arg(m_name));
    m_result = Unfinished;

    m_children = childrenByType<Component>();
    m_children << this;

    m_currentTestCase = this;

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
        Console::printError(QString("Test case '%1' does not define a 'run' method.").arg(m_name));
        m_result = TestCase::Fail;
        return TestCase::Destroyed;
    }

    // created() is a signal that even QML children can subscribe.
    for (Component* child : m_children)
    {
        QObject* attached = qmlAttachedPropertiesObject<TestCase>(child, false);
        if (attached != NULL)
        {
            m_attachedObjects << attached;
            QMetaObject::invokeMethod(attached, "created");
        }
    }
    emit created();

    return InitingTestCase;
}


TestCase::State TestCase::initingTestCaseStateFunction()
{
    this->connect(qmlEngine(this), &QQmlEngine::warnings, this, &TestCase::onQmlEngineWarnings);

    for (auto child : m_children)
    {
        child->initTestCase();
    }

    return InitingTestFunction;
}

TestCase::State TestCase::initingTestFunctionStateFunction()
{
    for (auto child : m_children)
    {
        child->initTestFunction();
    }

    return Running;
}

TestCase::State TestCase::runningStateFunction()
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

    m_timer.start();

    bool invocationSuccessful = QMetaObject::invokeMethod(this, "run");
    Q_ASSERT(invocationSuccessful == true);

    m_executionTime = m_timer.elapsed();
    m_timer.invalidate();

    return CleaningUpTestFunction;
}

TestCase::State TestCase::cleaningUpTestFunctionStateFunction()
{
    for (auto attached : m_attachedObjects)
    {
        QMetaObject::invokeMethod(attached, "finished");
    }
    emit finished();

    switch (m_result)
    {
    case Unfinished:
    case Success:
        Console::printToStdOut(QString("PASS, %1").arg(this->m_name));
        break;
    case Fail:
        Console::printToStdOut(QString("FAIL, %1, %2:%3, %4")
                               .arg(m_name)
                               .arg(m_callerFile)
                               .arg(m_callerLine)
                               .arg(m_message));
        break;
    }

    return CleaningUpTestCase;
}

TestCase::State TestCase::cleaningUpTestCaseStateFunction()
{
    // If nothing else has determined the result until now,
    // then it must be successful.
    if (m_result == Unfinished)
    {
        m_result = Success;
    }

    for (auto attached : m_attachedObjects)
    {
        QMetaObject::invokeMethod(attached, "destruction");
    }
    emit destruction();

    this->disconnect(qmlEngine(this), &QQmlEngine::warnings, this, &TestCase::onQmlEngineWarnings);

    return Destroyed;
}


/*
Aborts the test case and sets the test case result to result.

This function is to be called from C++ code.
*/
void TestCase::finishAndExit(TestCase::Result result, const QString& file, int line, const QString& message)
{

    // This seems to be the only way to inject an exception in the Qml engine from C++ context.
    QV4::ExecutionEngine* engine = QV8Engine::getV4(qmlEngine(this)->handle());

    m_result = result;
    m_callerFile = file;
    m_callerLine = line;
    m_message = message;

    switch (result)
    {
    case TestCase::Fail:
        engine->throwError(QString("TestCase::Fail"));
        break;
        // More results, i.e. Skip?
    default:
        // Should never be called with result = Success or anything else.
        Q_ASSERT_X(false, "TestCase::finishAndExit()", message.toLocal8Bit());
        break;
    }
}

void TestCase::waitUntilExpression(QJSValue expression, int milliseconds, const QString& file, int line)
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
                TestCase::instance()->finishAndExit(TestCase::Fail,  __FILE__, __LINE__, "timeout");
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

qint64 TestCase::elapsedTime() const
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


void TestCase::waitMilliseconds(int milliseconds, const QString& file, int line)
{
    m_callerFile = file;
    m_callerLine = line;
    QEventLoop eventLoop;
    QTimer::singleShot(milliseconds, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();
}

QString TestCase::qmlCallerFile()
{
    QV4::ExecutionEngine *engine = QV8Engine::getV4(qmlEngine(this)->handle());
    QV4::StackTrace trace = engine->stackTrace(2);
    QV4::StackFrame& frame = trace.last();
    return frame.source;
}

QString TestCase::qmlCallerLine()
{
    QV4::ExecutionEngine *engine = QV8Engine::getV4(qmlEngine(this)->handle());
    QV4::StackTrace trace = engine->stackTrace(2);
    QV4::StackFrame& frame = trace.last();

    return QString::number(frame.line);
}

/*
This slot is invoked whenever an exception in the QML engine observes
an exception.
*/
void TestCase::onQmlEngineWarnings(const QList<QQmlError> &warnings)
{
    Q_ASSERT(warnings.size() == 1);

    QV4::ExecutionEngine* engine = QV8Engine::getV4(qmlEngine(this)->handle());
    // qDebug() << "onQmlEngineWarnings Calling depth: " << engine->callDepth;

    const QQmlError& error = warnings.first();

    if (error.description() != "Error: TestCase::Fail")
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
        engine->throwError("TestCase::Fail");
    }
}

TestCaseAttached* TestCase::qmlAttachedProperties(QObject* object)
{
    return new TestCaseAttached(object);
}

Project* TestCase::project() const
{
    Project* project = qvariant_cast<Project*>(qmlContext(this)->contextProperty("project"));
    Q_ASSERT(project != NULL);
    return project;
}

TestCase* TestCase::instance()
{
    Q_ASSERT(!m_currentTestCase.isNull());
    return m_currentTestCase.data();
}

QString TestCase::workingDirectory() const
{
    return QDir(project()->workingDirectory()).absoluteFilePath(m_name);
}

