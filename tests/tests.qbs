import qbs

Project {
    condition : qbs.architecture === "x86_64"

    StaticLibrary {
        name: "qsttestlib"

        Depends { name: "Qt.core" }

        files: [
            "qsttest.cpp",
            "qsttest.h",
            "qsttestresults.cpp",
            "qsttestresults.h",
        ]

        cpp.defines: base.concat([
            'PROJECTPATH="' + path + '/../' + '"'
        ])

        Export {
            Depends { name: "cpp" }
            cpp.includePaths: path
        }
    }

    QtApplication {
        type: [ "application", "autotest" ]

        name : "auto"
        targetName: "qst-auto-test"
        destinationDirectory: "bin"

        Depends { name : "Qt.testlib" }
        Depends { name: "qsttestlib" }
        Depends { name: "testdata" }

        consoleApplication: true

        files : [
            "autotest.cpp",
            "autotest.h"
        ]

        Group {
            fileTagsFilter: product.type
            qbs.install: project.installTests
            qbs.installDir : "bin"
        }
    }

    QtApplication {
        type: [ "application" ]
        name : "pinprobe-test"
        targetName: "qst-pinprobe-test"
        destinationDirectory: "bin"

        Depends { name: "Qt.testlib" }
        Depends { name: "qsttestlib" }

        consoleApplication: true

        files : [
            "manualtest.cpp",
            "manualtest.h",
        ]

        Group {
            fileTagsFilter: product.type
            qbs.install: project.installTests
            qbs.installDir : "bin"
        }
    }

    Product {
        type: ["resources"]
        name: "testdata"
        files: ["testdata/**/*"]
    }

    AutotestRunner {
        builtByDefault: project.runAutotest
    }

}
