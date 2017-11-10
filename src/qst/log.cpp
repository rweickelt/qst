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
#include "log.h"
#include "console.h"

#include <QtCore/QPointer>

namespace {
    Log* singleLogInstance = 0;
}

Log* log() {
    return Log::instance();
}

/*
Config object must exist by the time, Log is created.
*/
Log::Log()
{
    m_level = WarningLevel;
}

Log* Log::instance()
{
    if (!singleLogInstance)
    {
        singleLogInstance = new Log();
    }
    return singleLogInstance;
}


void Log::printError(const QString& module, const QString& text)
{
    if (m_level < ErrorLevel)
    {
        return;
    }

    Console::printToStdError(QString("Error, %1: %2").arg(module, text));
}

void Log::printInfo(const QString& module, const QString& text)
{
    if (m_level < InfoLevel)
    {
        return;
    }

    Console::printToStdError(QString("Info, %1: %2").arg(module, text));
}

void Log::printWarning(const QString& module, const QString& text)
{
    if (m_level < WarningLevel)
    {
        return;
    }

    Console::printToStdError(QString("Warning, %1: %2").arg(module, text));
}
