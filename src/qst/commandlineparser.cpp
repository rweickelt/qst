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
#include "applicationoptions.h"
#include "console.h"
#include "commandlineparser.h"
#include "qst.h"

#include <QtDebug>
#include <QtCore/QCommandLineParser>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>

namespace {

const QString metaDescription =
        "Qst %1\n"
        "Usage: qst command [command parameters]\n"
        "\n"
        "Available commands:\n"
        "  run       Executes one or more test cases.\n"
        "  version   Diplays version information.\n";

const QString runCommandDescription =
        "Executes one or more test cases.";

}

CommandlineParser::CommandlineParser(QStringList arguments)
{
    ApplicationOptions* m_options = ApplicationOptions::instance();
    m_options->importPaths = QStringList(QCoreApplication::applicationDirPath() + "/../share/qst/imports");

    if (arguments.length() < 2)
    {
        m_errorString = "Too few arguments";
        return;
    }

    QString command = arguments.at(1);

    // Workaround to adopt QCommandLineParser to our complex
    // application with a positional argument before specific
    // options and other positional arguments.
    //arguments[0] = arguments[0] + command;
    // Todo: doesnt work. We need another work-around even before initializing
    // QCoreApplication
    arguments.removeAt(1);

    if (command == "run")
    {
        m_options->command = ApplicationOptions::RunCommand;

        QCommandLineOption profileOption = {
            { "p", "profile" },
            "Load a profile from <name>.json and attach it to the QML root context. "
            "Matching profiles are searched in the directories specified by -P or in the "
            "project directory or in the config directory's profile sub-directory "
            " (in this order).",
            "name"
        };

        QCommandLineOption profilePathsOption = {
            { "P", "profile-directory" },
            "When looking for profiles, search first in <path>. "
            "This option might be given multiple times.",
            "path"
        };

        QCommandLineOption projectFilepathOption = {
            { "f", "file" },
            "Uses <file> as project file. If <file> is a directory and contains a single "
            "file ending in '.qml', that file will be used. If this option is not given "
            "at all, the behavior is the same as for '--file .'.",
            "file"
        };

        QCommandLineOption importPathsOption = {
            { "I", "import" },
            "Adds <path> as a directory where the engine searches for installed modules "
            "in a URL-based directory structure.\n"
            "The path may be a local filesystem directory, a Qt Resource path (:/imports), "
            "a Qt Resource url (qrc:/imports) or a URL.",
            "path"
        };

        QCommandLineOption workingDirectoryOption = {
            { "d", "working-directory" },
            "Use <path> as working directory during test case execution. "
            "If the directory does not exist, it is being created. "
            "By default, a temporary directory with a random name is used.",
            "path"
        };

        QCommandLineParser parser;
        parser.setApplicationDescription(runCommandDescription);
        parser.addOption(workingDirectoryOption);
        parser.addOption(projectFilepathOption);
        parser.addHelpOption();
        parser.addOption(importPathsOption);
        parser.addOption(profileOption);
        parser.addOption(profilePathsOption);

        parser.process(arguments);

        if (parser.isSet(projectFilepathOption))
        {
            QFileInfo projectFilepath = parser.value(projectFilepathOption);
            if (!projectFilepath.exists())
            {
                m_errorString = QString("Specified project filepath '%1' does not exist.")
                        .arg(parser.value(projectFilepathOption));
                return;
            }

            if (projectFilepath.isDir())
            {
                QDir projectDir = projectFilepath.absoluteDir();
                QStringList files = projectDir.entryList(QStringList("*.qml"), QDir::Files);
                if (files.empty())
                {
                    m_errorString = QString("Specified project filepath '%1' does not contain any project file.")
                            .arg(parser.value(projectFilepathOption));
                    return;
                }
                else if (files.size() > 1)
                {
                    m_errorString = QString("Specified project filepath '%1' contains multiple files.")
                            .arg(parser.value(projectFilepathOption));
                    return;
                }
                else
                {
                    m_options->projectFilepath = files.first();
                }
            }
            else if (projectFilepath.isFile())
            {
                m_options->projectFilepath = projectFilepath.absoluteFilePath();
            }
            else
            {
                Q_ASSERT(false);
            }
        }
        else // --file not set
        {
            QDir projectDir = QDir();
            QStringList files = projectDir.entryList(QStringList("*.qml"), QDir::Files);
            if (files.empty())
            {
                m_errorString = QString("Current directory does not contain any project file.");
                return;
            }
            else if (files.size() > 1)
            {
                m_errorString = QString("Current directory contains multiple qml files.");
                return;
            }
            else
            {
                m_options->projectFilepath = files.first();
            }
        }

        if (parser.isSet(importPathsOption))
        {
            m_options->importPaths << parser.values(importPathsOption);
        }

        if (parser.isSet(workingDirectoryOption))
        {
            m_options->workingDirectory = QDir(parser.value(workingDirectoryOption)).absolutePath();
        }

        if (parser.isSet(profileOption))
        {
            QStringList profileNames = parser.values(profileOption);
            if (profileNames.length() > 1)
            {
                m_errorString = QString("--profile/-p can only specified once.");
                return;
            }
            m_options->profile = profileNames.first();
        }

        if (parser.isSet(profilePathsOption))
        {
            m_options->profilePaths = parser.values(profilePathsOption);
            for (int i = 0; i < m_options->profilePaths.length(); i++)
            {
                m_options->profilePaths[i] = QDir(m_options->profilePaths[i]).absolutePath();
            }
        }
        // Add project path to profile search paths
        m_options->profilePaths << QFileInfo(m_options->projectFilepath).absolutePath();
    }
    else if (command == "help")
    {
        m_options->command = ApplicationOptions::HelpCommand;
    }
    else if (command == "version")
    {
        m_options->command = ApplicationOptions::VersionCommand;
    }
    else
    {
        m_errorString = QString("Command '%1' not known.").arg(command);
        return;
    }
}

QString CommandlineParser::helpText() const
{
    return metaDescription.arg(QCoreApplication::applicationVersion());
}


