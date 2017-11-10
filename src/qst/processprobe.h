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
#ifndef PROCESSPROBE_H
#define PROCESSPROBE_H

#include <QObject>
#include <QProcess>
#include <QStringList>
#include <QVariant>
#include "component.h"

class ProcessProbe : public Component
{
    Q_OBJECT

public:
    enum State {
        NotRunning = QProcess::NotRunning,
        Starting = QProcess::Starting,
        Running = QProcess::Running
    };
    Q_ENUMS(State)

    enum ProcessError {
        FailedToStart = QProcess::FailedToStart,
        Crashed = QProcess::Crashed,
        Timedout = QProcess::Timedout,
        WriteError = QProcess::WriteError,
        ReadError = QProcess::ReadError,
        UnknownError = QProcess::UnknownError
    };
    Q_ENUMS(ProcessError)

    Q_PROPERTY(QString program MEMBER m_program)
    Q_PROPERTY(QStringList arguments MEMBER m_arguments)
    Q_PROPERTY(int exitCode READ exitCode STORED false NOTIFY exitCodeChanged)
    Q_PROPERTY(QString workingDirectory MEMBER m_workingDirectory)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)

    Q_INVOKABLE QString errorString() const;
    Q_INVOKABLE QString readAllStandardError();
    Q_INVOKABLE QString readAllStandardOutput();
    Q_INVOKABLE bool waitForFinished(int milliseconds);
    Q_INVOKABLE bool waitForStarted(int milliseconds);

    explicit ProcessProbe(QObject *parent = 0);
    int exitCode() const;
    State state() const;

    void classBegin();
    void componentComplete();

public slots:
    void start();
    void terminate();

signals:
    void errorOccured(ProcessProbe::ProcessError error);
    void exitCodeChanged();
    void started();
    void finished();
    void stateChanged();

protected slots:
    void onProcessErrorOccurred(QProcess::ProcessError error);
    void onProcessStarted();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessStateChanged(QProcess::ProcessState);

protected:
    void setState(State);
    void initTestCase();

private:
    QProcess m_process;
    QString m_program;
    QStringList m_arguments;
    QString m_workingDirectory;
};

#endif // PROCESSPROBE_H
