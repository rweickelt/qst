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
#ifndef TESTCASE_H
#define TESTCASE_H

#include <QtCore/QElapsedTimer>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QTimer>

#include <QtQml/QJSValue>
#include <QtQml/QQmlError>
#include <QtQml/QQmlComponent>

#include "component.h"

class Project;
class TestcaseAttached;

class Testcase : public Component
{
    Q_OBJECT
    Q_DISABLE_COPY(Testcase)

    friend class ProjectResolver;
    friend class TestRunner;

public:
    enum State
    {
        Uninitialized = 0,
        InitingTestCase = 1,
        InitingTestFunction = 2,
        Running = 3, //
        CleaningUpTestFunction = 4,
        CleaningUpTestCase = 5,
        Destroyed = 6
    };
    Q_ENUM(State)

    enum Result
    {
        Unfinished = -2,
        Fail = -1,
        Success = 0,
    };
    Q_ENUM(Result)

    Q_PROPERTY(qint64 elapsedTime READ elapsedTime)
    Q_PROPERTY(Result result MEMBER m_result READ result)
    Q_PROPERTY(State state READ state)
    Q_PROPERTY(QString workingDirectory READ workingDirectory CONSTANT)
    Q_PROPERTY(QString message MEMBER m_message)

    void componentComplete() override;

    template <typename T>
    QList<T*> childrenByType() const;

    void registerChild(Component* component);

    static Testcase* instance();

signals:
    // Emitted after the test case constructor has been finished.
    void created();

    // Emitted before the test case destructor is executed.
    void destruction();

    // Emitted everytime before a test function is invoked.
    void started();

    // Emitted everytime after a test function has been invoked.
    void finished();

public slots:
    void finishAndExit(Testcase::Result result, const QString& file = "", int line = 0, const QString& message = "");


protected slots:
    Result exec();
    void onQmlEngineWarnings(const QList<QQmlError> &warnings);

protected:
    Q_INVOKABLE void waitMilliseconds(int milliseconds, const QString& file, int line);
    Q_INVOKABLE void waitUntilExpression(QJSValue expression, int milliseconds, const QString& file, int line);

    void initTestCase() override;
    Project* project() const;

public:
    Testcase(QObject *parent = 0);
    QString errorString() const;
    bool hasErrors() const;
    qint64 elapsedTime() const;
    Result result() const;
    State state() const;
    QString workingDirectory() const;

    static TestcaseAttached* qmlAttachedProperties(QObject *);

private:
    State unitializedStateFunction();
    State initingTestCaseStateFunction();
    State initingTestFunctionStateFunction();
    State runningStateFunction();
    State cleaningUpTestFunctionStateFunction();
    State cleaningUpTestCaseStateFunction();
    State destroyedStateFunction();

    Result m_result;
    State m_state;
    State m_nextState;
    bool m_transitionPending;
    QList<Component*> m_children;
    QList<QObject*> m_attachedObjects;

    int m_callerLine;
    QString m_callerFile;
    QString m_message;
    QElapsedTimer m_timer;
    qint64 m_executionTime;

    static QPointer<Testcase> m_currentTestCase;
    QString m_errorString;
};

Q_DECLARE_METATYPE(Testcase::State)
QML_DECLARE_TYPEINFO(Testcase, QML_HAS_ATTACHED_PROPERTIES)

inline QString Testcase::errorString() const { return m_errorString; }
inline bool Testcase::hasErrors() const { return !m_errorString.isEmpty(); }
inline Testcase::Result Testcase::result() const { return m_result; }
inline Testcase::State Testcase::state() const { return m_state; }
inline void Testcase::registerChild(Component* component) { m_children.append(component); }

template <typename T>
QList<T*> Testcase::childrenByType() const
{
    QList<T*> result;
    for (auto child : m_children)
    {
        if (child->metaObject()->inherits(&T::staticMetaObject))
        {
            result << qobject_cast<T*>(child);
        }
    }
    return result;
}

#endif // TESTCASE_H
