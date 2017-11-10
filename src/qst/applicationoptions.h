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
#ifndef APPLICATIONOPTIONS_H
#define APPLICATIONOPTIONS_H

#include <QtCore/QString>
#include <QtCore/QStringList>

struct ApplicationOptions
{
    enum Command
    {
        InvalidCommand,
        HelpCommand,
        NoCommand,
        RunCommand,
        VersionCommand
    };

    Command command;
    QStringList importPaths;
    QString projectFilepath;
    QString workingDirectory;

    static ApplicationOptions* instance();

protected:
    ApplicationOptions();

private:
    ApplicationOptions(const ApplicationOptions&);
    ApplicationOptions& operator=(const ApplicationOptions&);

    friend class ApplicationOptionsCreator;
};

#endif // APPLICATIONOPTIONS_H
