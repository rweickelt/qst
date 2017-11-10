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
#include <QtCore/QTextStream>

class Log
{
public:
    enum LogLevel
    {
        SilentLevel = 0,
        ErrorLevel = 1,
        WarningLevel = 2,
        InfoLevel = 3
    };

    static Log* instance();

    void printError(const QString& module, const QString& text);
    void printInfo(const QString& module, const QString& text);
    void printWarning(const QString& module, const QString& text);

private:
    Log();
    Q_DISABLE_COPY(Log)

    LogLevel m_level;
    QFile m_logFile;
    QTextStream m_logStream;
};

Log* log();

#endif // LOG_H
