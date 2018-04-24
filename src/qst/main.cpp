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
#include "dimension.h"
#include "file.h"
#include "xds.h"
#include "plaintextlogger.h"
#include "matrix.h"
#include "jobexpander.h"
#include "pinprobe.h"
#include "processprobe.h"
#include "profileloader.h"
#include "project.h"
#include "projectresolver.h"
#include "proxylogger.h"
#include "qst.h"
#include "testcase.h"
#include "testcaseattached.h"
#include "jobrunner.h"
#include "textfile.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QPointer>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>

#define STRINGIFY(x) #x
#define AS_STRING(x) STRINGIFY(x)

#define CHECK_FOR_ERROR(object) \
    if (object.hasError()) \
    { \
        Console::printError("Error: " + object.errorString()); \
        ::exit(qst::ExitApplicationError); \
    }

#define CHECK_FOR_ERRORS(object) \
    if (object.hasErrors()) { \
        for (const auto& error : object.errors()) { \
            Console::printError("Error: " + error); \
        } \
        ::exit(qst::ExitApplicationError); \
    }


void execRunCommand();

int main(int argc, char *argv[])
{
    // Disable QML compiler.
    // We don't want to pollute our directories with .qmlc files
    qputenv("QML_DISABLE_DISK_CACHE", "1");

    QCoreApplication app(argc, argv);

#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
    app.setApplicationName(QLatin1String("Qst"));
#else
    app.setApplicationName(QLatin1String("qst"));
#endif
    app.setApplicationVersion(QLatin1String(AS_STRING(QST_VERSION)));

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

    qmlRegisterCustomType<Dimension>("qst", 1,0, "Dimension", new DimensionParser());
    qmlRegisterType<Matrix>("qst", 1,0, "Matrix");
    qmlRegisterType<Component>("qst", 1,0, "Component");
    qmlRegisterType<Testcase>("qst", 1,0, "Testcase");
    qmlRegisterType<TestcaseAttached>();
    qmlRegisterType<PinProbe>("qst", 1,0, "PinProbe");
    qmlRegisterType<ProcessProbe>("qst", 1,0, "ProcessProbe");
    qmlRegisterType<Project>("qst", 1,0, "Project");
    qmlRegisterType<QstService>("qst", 1, 0, "QstService");
    qmlRegisterUncreatableType<TextFile>("qst", 1, 0, "TextFile", "TextFile can only be created in a JS context");
    TextFile::registerJSType(&engine);

    qRegisterMetaType<Testcase::State>();
    qRegisterMetaType<QmlContext>();
    qRegisterMetaType<QList<QmlContext> >();

    qmlRegisterSingletonType<File>("qst", 1,0, "File", &File::createSingleInstance);
    qmlRegisterSingletonType<Xds>("ti", 1, 0, "Xds", &Xds::createSingleInstance);

    PlaintextLogger plaintextLogger;
    ProxyLogger::instance()->registerLogger(&plaintextLogger);

    ProfileLoader profileLoader(options->profilePaths);
    QVariant profile = profileLoader.loadProfile(options->profile);
    CHECK_FOR_ERROR(profileLoader);
    engine.rootContext()->setContextProperty("profile", profile);

    ProjectResolver resolver(&engine);
    resolver.loadRootFile(options->projectFilepath);
    CHECK_FOR_ERRORS(resolver);

    JobExpander expander(resolver.matrices(), resolver.testcases());
    CHECK_FOR_ERROR(expander);

    JobRunner runner(resolver.project(), expander.jobs().values(), expander.tags());
    CHECK_FOR_ERROR(runner);

    runner.execTestCases();
}


