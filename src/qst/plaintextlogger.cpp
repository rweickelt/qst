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

#include "plaintextlogger.h"
#include "console.h"

PlaintextLogger::PlaintextLogger()
{

}

void PlaintextLogger::print(const LogInfo& info)
{
    QString type;
    switch (info.type)
    {
    case LogInfo::Error:
        Console::printToStdOut(QString("ERROR, %1, %2, %3:%4, %5")
                               .arg(info.test, info.component, info.file, QString::number(info.line), info.message));
        break;
    case LogInfo::Fail:
        Console::printToStdOut(QString("FAIL, %1, %2, %3:%4, %5")
                               .arg(info.test, info.component, info.file, QString::number(info.line), info.message));
        break;
    case LogInfo::Info:
        Console::printToStdOut(QString("INFO, %1, %2, %3:%4, %5")
                               .arg(info.test, info.component, info.file, QString::number(info.line), info.message));
        break;
    case LogInfo::Success:
        Console::printToStdOut(QString("PASS, %1,,,").arg(info.test));
        break;
    case LogInfo::Warning:
        Console::printToStdOut(QString("WARN, %1, %2, %3:%4, %5")
                               .arg(info.test, info.component, info.file, QString::number(info.line), info.message));
        break;
    }
}
