import qbs

QtApplication {
    name : "qst-application"

    condition : { return qbs.architecture.startsWith("x86") }

    Depends { name : "Qt.qml"; }
    Depends { name : "Qt.serialport"; }
    Depends { name : "cpp" }
    Depends { name : "Qt.qml-private"; }
    Depends { name : "shared-types" }

    cpp.cxxLanguageVersion : "c++14"
    cpp.rpaths: [
        "$ORIGIN/../lib/"
    ]
    cpp.defines: [
        "QST_COMMIT="  + project.commit,
        "QST_VERSION=" + project.version,
    ]

    consoleApplication : true
    targetName : "qst"

    files : [
        "applicationoptions.cpp",
        "applicationoptions.h",
        "commandlineparser.cpp",
        "commandlineparser.h",
        "component.cpp",
        "component.h",
        "console.cpp",
        "console.h",
        "file.cpp",
        "file.h",
        "logger.cpp",
        "logger.h",
        "main.cpp",
        "parsereventhandler.h",
        "plaintextlogger.cpp",
        "plaintextlogger.h",
        "profileloader.cpp",
        "profileloader.h",
        "projectresolver.cpp",
        "projectresolver.h",
        "proxylogger.cpp",
        "proxylogger.h",
        "qst.cpp",
        "qsterror.cpp",
        "qsterror.h",
        "qstextensionplugin.cpp",
        "qstextensionplugin.h",
        "rochostcontroller.cpp",
        "rochostcontroller.h",
        "rochostobject.cpp",
        "rochostobject.h",
        "signallog.cpp",
        "signallog.h",
        "qst.h",
        "pinprobe.cpp",
        "pinprobe.h",
        "processprobe.cpp",
        "processprobe.h",
        "project.cpp",
        "project.h",
        "stpsocket.cpp",
        "stpsocket.h",
        "testcase.cpp",
        "testcase.h",
        "testcaseattached.cpp",
        "testcaseattached.h",
        "testrunner.cpp",
        "testrunner.h",
        "textfile.cpp",
        "textfile.h",
        "xds.cpp",
        "xds.h",
    ]

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir : "bin"
    }

    destinationDirectory : "bin"

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: product.sourceDirectory
    }
}

