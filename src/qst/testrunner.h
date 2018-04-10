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
#ifndef TESTCASERUNNER_H
#define TESTCASERUNNER_H

#include "testcase.h"

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPointer>

class Project;

class QQmlEngine;

class TestRunner : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(TestRunner)
public:
    TestRunner(Project* project, const QList<QPointer<Testcase> >& testCases);
    QString errorString() const;
    bool hasError() const;

public slots:
    void execTestCases();

private:
    void createWorkingDirectory();

    QString m_errorString;
    QPointer<Project> m_project;
    QList<QPointer<Testcase> > m_testCases;
    QList<Testcase::Result> m_results;
};

inline QString TestRunner::errorString() const { return m_errorString; }
inline bool TestRunner::hasError() const { return !m_errorString.isEmpty(); }

#endif // TESTCASERUNNER_H
