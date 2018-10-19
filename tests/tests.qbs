import qbs

Project {
    condition : qbs.architecture.lastIndexOf("x86") === 0

    references: [
        "auto/auto.qbs"
    ]

    StaticLibrary {
        name: "qsttestlib"

        Depends { name: "Qt.core" }
        Depends { name: "Qt.testlib" }

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

    AutotestRunner {
        builtByDefault: project.runAutotest
    }

}
