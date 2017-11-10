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
#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <QtCore/QString>

class QStringList;

/*
Handles command line parsing and console output.
*/
class CommandlineParser
{
    Q_DISABLE_COPY(CommandlineParser)

public:
    CommandlineParser(QStringList arguments);
    QString errorString() const;
    bool hasErrors() const;
    QString helpText() const;

    QString m_errorString;
};

inline QString CommandlineParser::errorString() const { return m_errorString; }
inline bool CommandlineParser::hasErrors() const { return !m_errorString.isEmpty(); }

#endif // COMMANDLINEPARSER_H
