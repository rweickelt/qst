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
#include "commandlineparser.h"
#include "component.h"
#include "console.h"
#include "file.h"
#include "launchpad.h"
#include "plaintextlogger.h"
#include "pinprobe.h"
#include "processprobe.h"
#include "project.h"
#include "projectresolver.h"
#include "proxylogger.h"
#include "qst.h"
#include "testcase.h"
#include "testcaseattached.h"
#include "testrunner.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QPointer>
#include <QtQml/QQmlEngine>

void execRunCommand();

int main(int argc, char *argv[])
{
    // Disable QML compiler.
    // We don't want to pollute our directories with .qmlc files
    qputenv("QML_DISABLE_DISK_CACHE", "1");

    QCoreApplication app(argc, argv);
    CommandlineParser cli(app.arguments());
    if (cli.hasErrors())
    {
        Console::printError(QString("Error: %1").arg(cli.errorString()));
        Console::printToStdOut(cli.helpText());
        return qst::ExitApplicationError;
    }

    ApplicationOptions* options = ApplicationOptions::instance();
    switch (options->command)
    {
    case ApplicationOptions::RunCommand:
        QTimer::singleShot(0, [](){ execRunCommand(); });
        return app.exec();
    case ApplicationOptions::HelpCommand:
        Console::printToStdOut(cli.helpText());
        return qst::ExitNormal;
    case ApplicationOptions::VersionCommand:
        Console::printToStdOut(QCoreApplication::applicationVersion());
        return qst::ExitNormal;
    default:
        Q_ASSERT(false);
    }
}

void execRunCommand()
{
    ApplicationOptions* options = ApplicationOptions::instance();
    QQmlEngine engine;
    engine.connect(&engine, &QQmlEngine::quit, QCoreApplication::instance(), QCoreApplication::quit);

    for (const auto& path : options->importPaths)
    {
        engine.addImportPath(path);
    }

    qmlRegisterType<Component>("qst", 1,0, "Component");
    qmlRegisterType<Testcase>("qst", 1,0, "Testcase");
    qmlRegisterType<TestcaseAttached>();
    qmlRegisterType<PinProbe>("qst", 1,0, "PinProbe");
    qmlRegisterType<ProcessProbe>("qst", 1,0, "ProcessProbe");
    qmlRegisterType<Project>("qst", 1,0, "Project");
    qmlRegisterType<QstService>("qst", 1, 0, "QstService");

    qRegisterMetaType<Testcase::State>();

    qmlRegisterSingletonType<File>("qst", 1,0, "File", &File::createSingleInstance);
    qmlRegisterSingletonType<Launchpad>("ti", 1, 0, "Launchpad", &Launchpad::createSingleInstance);

    PlaintextLogger plaintextLogger;
    ProxyLogger::instance()->registerLogger(&plaintextLogger);

    ProjectResolver resolver(&engine, options->projectFilepath);
    resolver.loadRootFile();
    if (resolver.hasErrors())
    {
        for (const QString& error : resolver.errors())
        {
            Console::printError(error);
        }
        ::exit(qst::ExitApplicationError);
    }

    TestRunner runner(resolver.testcases());
    runner.execTestCases();
}


