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

#include "proxylogger.h"

#include <QtCore/QGlobalStatic>
#include <QtCore/QMutexLocker>

namespace {
    Q_GLOBAL_STATIC(ProxyLogger, singleLoggerObject)
}

ProxyLogger::ProxyLogger()
{

}

void ProxyLogger::registerLogger(Logger* logger)
{
    Q_ASSERT(logger != NULL);
    m_loggers.append(logger);
}

void ProxyLogger::print(const LogInfo& info)
{
    QMutexLocker lock(&m_mutex);
    for (const auto logger : m_loggers)
    {
        logger->print(info);
    }
}

ProxyLogger* ProxyLogger::instance()
{
    return singleLoggerObject();
}
