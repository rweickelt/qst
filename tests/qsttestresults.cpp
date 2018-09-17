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

#include "qsttestresults.h"

#include <QtCore/QByteArray>
#include <QtCore/QRegularExpression>

bool QstTestResults::contains(const QString& name) const
{
    return m_data.contains(name);
}

QstTestResults QstTestResults::fromQstOutput(const QByteArray& text)
{
    QstTestResults results;

    results.m_stdOut = text;
    results.m_passCount = 0;
    results.m_failCount = 0;

    QStringList lines = QString::fromLatin1(text).trimmed().split('\n');
    QRegularExpression parser(
                "^(?<result>\\w+)"
                ",\\s*(?<name>[\\w\\-\\[\\]\\s]+)"
                ",\\s*(?<component>[\\w\\-]*)"
                "(,\\s*(?<position>.*)"
                ",\\s*(?<message>.*))?$"
    );

    for (const QString& line : lines)
    {
        QRegularExpressionMatch parsed = parser.match(line);
        if (parsed.hasMatch() || parsed.hasPartialMatch())
        {
            QstOutput newEntry;
            newEntry.result = parsed.captured("result");
            newEntry.name = parsed.captured("name");
            newEntry.component = parsed.captured("component");
            newEntry.location = parsed.captured("position");
            newEntry.message = parsed.captured("message");

            if (newEntry.result == "PASS")
            {
                results.m_passCount++;
                results.m_data[parsed.captured("name")] = newEntry;
            }
            else if (newEntry.result == "FAIL")
            {
                results.m_failCount++;
                results.m_data[parsed.captured("name")] = newEntry;
            }
            else if (newEntry.result == "INFO")
            {
                results.m_info.insert(parsed.captured("name"), newEntry);
            }
            else
            {
                Q_ASSERT(false);
            }

            results.m_names.append(newEntry.name);
        }
        else
        {
            // Error before any test case started.
        }
    }

    return results;
}

int QstTestResults::failCount() const
{
    return m_failCount;
}

int QstTestResults::passCount() const
{
    return m_passCount;
}

bool QstTestResults::hasFailed(const QString &name) const
{
    if (m_data.contains(name))
    {
        return m_data[name].result == "FAIL";
    }
    else
    {
        return false;
    }
}

bool QstTestResults::hasPassed(const QString &name) const
{
    if (m_data.contains(name))
    {
        return m_data[name].result == "PASS";
    }
    else
    {
        return false;
    }
}

int QstTestResults::indexOf(const QString& name) const
{
    return m_names.indexOf(name);
}

QStringList QstTestResults::names() const
{
    return m_names;
}

const QstOutput QstTestResults::output(const QString& name) const
{
    return m_data.value(name);
}

QByteArray QstTestResults::stdOut() const
{
    return m_stdOut;
}
