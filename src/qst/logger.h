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
#ifndef LOG_H
#define LOG_H

#include <QtGlobal>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QTextStream>

struct LogInfo
{
    enum Type {
        Error,
        Fail,
        Info,
        Success,
        Warning
    };

    QString test;
    QString component;
    QString file;
    int line = 0;
    QString message;
    Type type = Info;
};


class Logger
{
public:
    Logger() {}
    virtual void print(const LogInfo& info) = 0;

private:
    Q_DISABLE_COPY(Logger)
};

#endif // LOG_H
