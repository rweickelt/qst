/****************************************************************************
 **
 ** Copyright (C) 2017-2018 The Qst project.
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
#include "logger.h"
#include "processprobe.h"
#include "qst.h"
#include "testcase.h"
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QEventLoop>
#include <QtQml/QQmlEngine>

ProcessProbe::ProcessProbe(QObject *parent) : Component(parent)
{
    connect(&m_process, &QProcess::errorOccurred, this, &ProcessProbe::onProcessErrorOccurred);
    connect(&m_process, &QProcess::started, this, &ProcessProbe::onProcessStarted);
    connect(&m_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &ProcessProbe::onProcessFinished);
    connect(&m_process, &QProcess::stateChanged, this, &ProcessProbe::onProcessStateChanged);
}

void ProcessProbe::classBegin()
{
    Component::classBegin();
}

void ProcessProbe::componentComplete()
{
    Component::componentComplete();

    if (m_workingDirectory.isEmpty())
    {
        m_workingDirectory = testCase()->workingDirectory();
    }
    Q_ASSERT(!m_workingDirectory.isEmpty());
}

// TODO Need to replace those conversions by QTextStream
QString ProcessProbe::readAllStandardError()
{
    return QString(m_process.readAllStandardError());
}

QString ProcessProbe::readAllStandardOutput()
{
    return QString(m_process.readAllStandardOutput());
}

QString ProcessProbe::errorString() const
{
    return m_process.errorString();
}

int ProcessProbe::exitCode() const
{
    return m_process.exitCode();
}

ProcessProbe::State ProcessProbe::state() const
{
    return static_cast<State>(m_process.state());
}

void ProcessProbe::initTestCase()
{

}

void ProcessProbe::start()
{
    m_process.setWorkingDirectory(m_workingDirectory);
    m_process.start(m_program, m_arguments);

    // Wait 50ms
    m_process.waitForStarted(50);
}


void ProcessProbe::terminate()
{
    m_process.terminate();
}

bool ProcessProbe::waitForStarted(int milliseconds)
{
    if (m_process.state() != QProcess::Starting)
    {
        return true;
    }

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    timer.setTimerType(Qt::PreciseTimer);
    timer.start(milliseconds);
    connect(&m_process, &QProcess::started, &loop, [&loop](){ loop.exit(0); });
    connect(&timer, &QTimer::timeout, &loop, [&loop]{ loop.exit(1); });
    return loop.exec() == 0;
}

bool ProcessProbe::waitForFinished(int milliseconds)
{
    if (m_process.state() != QProcess::Running)
    {
        return false;
    }

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    timer.setTimerType(Qt::PreciseTimer);
    timer.start(milliseconds);
    connect(&m_process, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
            &loop, [&loop](int){ loop.exit(0); });
    connect(&timer, &QTimer::timeout, &loop, [&loop]{ loop.exit(1); });
    return loop.exec() == 0;
}

void ProcessProbe::onProcessErrorOccurred(QProcess::ProcessError error)
{
    // Do not abort the test case when a custom signal handler
    // is attached.
    if (receivers(SIGNAL(errorOccured(ProcessProbe::ProcessError))) > 0)
    {
        emit this->errorOccured(static_cast<ProcessProbe::ProcessError>(error));
    }
    else
    {
        QST_VERIFY(false,
                   QString("Could not invoke '%1 %2' in folder '%3': %4")
                        .arg(m_program)
                        .arg(m_arguments.join(' '))
                        .arg(m_process.workingDirectory())
                        .arg(m_process.errorString()));
    }
}

void ProcessProbe::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);

    emit exitCodeChanged();
    emit this->finished();
}


void ProcessProbe::onProcessStarted()
{
    emit this->started();
}

void ProcessProbe::onProcessStateChanged(QProcess::ProcessState newState)
{
    Q_UNUSED(newState);
    emit this->stateChanged();
}
