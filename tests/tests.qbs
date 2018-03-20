import qbs

Project {
    condition : { return qbs.architecture.startsWith("x86") }

    StaticLibrary {
        name: "qsttestlib"

        Depends { name: "Qt.core" }
        Depends { name: "qst-application" }

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
            Depends { name: "qst-application" }
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
            "autotest.h",
        ]

        Group {
            fileTagsFilter: product.type
            qbs.install: project.installTests
            qbs.installDir : "bin"
        }
    }

    QtApplication {
        type: [ "application" ]
        name : "launchpad-probe-test"
        targetName: "qst-launchpad-probe-test"
        destinationDirectory: "bin"

        Depends { name: "Qt.testlib" }
        Depends { name: "qsttestlib" }

        consoleApplication: true

        files : [
            "launchpadprobetest.cpp",
            "launchpadprobetest.h",
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
