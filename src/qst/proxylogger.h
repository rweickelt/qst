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
#ifndef PROXYLOGGER_H
#define PROXYLOGGER_H

#include "logger.h"

#include <QtCore/QMutex>
#include <QtCore/QList>

class ProxyLogger : public Logger
{
public:
    ProxyLogger();
    void registerLogger(Logger* logger);
    virtual void print(const LogInfo& info) override;

    static ProxyLogger* instance();

private:
    QList<Logger*> m_loggers;
    QMutex m_mutex;
};

#endif // PROXYLOGGER_H
