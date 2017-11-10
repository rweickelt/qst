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
#ifndef TESTCASERUNNER_H
#define TESTCASERUNNER_H

#include "testcase.h"

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPointer>

class QQmlEngine;

class TestRunner : public QObject
{
    Q_OBJECT
public:
    TestRunner(const QList<QPointer<TestCase> >& testCases);

public slots:
    void execTestCases();

private:
    QList<QPointer<TestCase> > m_testCases;
    QList<TestCase::Result> m_results;
};

#endif // TESTCASERUNNER_H
