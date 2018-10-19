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

#ifndef QSTTESTRESULTS_H
#define QSTTESTRESULTS_H

#include <QtCore/QHash>
#include <QtCore/QString>

class QByteArray;

struct QstOutput {
    QString name;
    QString component;
    QString result;
    QString location;
    QString message;
};

class QstTestResults
{
public:
    // We are fine with implicitly created constructors and assignment

    static QstTestResults fromQstOutput(const QByteArray& text);

    bool hasFailed(const QString& name) const;
    bool hasPassed(const QString& name) const;
    bool contains(const QString& name) const;
    int indexOf(const QString& name) const;
    QStringList names() const;
    const QstOutput output(const QString& name) const;
    QByteArray stdOut() const;

    int failCount() const;
    int passCount() const;

private:
    QHash<QString, QstOutput>  m_data;
    QMultiHash<QString, QstOutput> m_info;
    QStringList m_names;
    int m_failCount;
    int m_passCount;
    QByteArray m_stdOut;
};


#endif // QSTTESTRESULTS_H
