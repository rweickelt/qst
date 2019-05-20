import qbs

QtApplication {
    name : "qst-application"

    condition : qbs.architecture.startsWith("x86")

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
        "dependency.cpp",
        "dependency.h",
        "dependencyresolver.cpp",
        "dependencyresolver.h",
        "depends.cpp",
        "depends.h",
        "dimension.cpp",
        "dimension.h",
        "directedgraph.h",
        "exports.cpp",
        "exports.h",
        "file.cpp",
        "file.h",
        "hierarchyvalidator.cpp",
        "hierarchyvalidator.h",
        "job.cpp",
        "job.h",
        "jobdispatcher.cpp",
        "jobdispatcher.h",
        "jobserver.cpp",
        "jobserver.h",
        "logger.cpp",
        "logger.h",
        "main.cpp",
        "matrix.cpp",
        "matrix.h",
        "matrixresolver.cpp",
        "matrixresolver.h",
        "plaintextlogger.cpp",
        "plaintextlogger.h",
        "profileloader.cpp",
        "profileloader.h",
        "projectdatabase.cpp",
        "projectdatabase.h",
        "projectresolver.cpp",
        "projectresolver.h",
        "projecttree.cpp",
        "projecttree.h",
        "proxylogger.cpp",
        "proxylogger.h",
        "qst.cpp",
        "qstdocument.h",
        "qstitem.cpp",
        "qstitem.h",
        "qstitemvisitor.cpp",
        "qstitemvisitor.h",
        "resource.cpp",
        "resource.h",
        "resourceitem.cpp",
        "resourceitem.h",
        "rochostcontroller.cpp",
        "rochostcontroller.h",
        "rochostobject.cpp",
        "rochostobject.h",
        "serialjobscheduler.cpp",
        "serialjobscheduler.h",
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
        "tag.cpp",
        "tag.h",
        "testcase.cpp",
        "testcase.h",
        "testcaseattached.cpp",
        "testcaseattached.h",
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

