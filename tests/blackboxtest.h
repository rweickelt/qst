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

#ifndef BLACKBOXTEST_H
#define BLACKBOXTEST_H

#include <QObject>
#include <QtTest/QtTest>

class BlackboxTest : public QObject
{
    Q_OBJECT
public:
    explicit BlackboxTest(QObject *parent = 0);

private slots:
    void project();
    void testCase();
    void testCaseName();
    void errorHandling();
    void processProbe();
    void workingDirectory_data();
    void workingDirectory();
    void pinProbe();
};

#endif // BLACKBOXTEST_H
