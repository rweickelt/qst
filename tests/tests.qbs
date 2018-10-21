import qbs

Project {
    condition : qbs.architecture.lastIndexOf("x86") === 0

    references: [
        "auto/auto.qbs",
        "manual/manual.qbs"
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

    AutotestRunner {
        builtByDefault: project.runAutotest
    }

}
