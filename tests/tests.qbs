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

        name : "autotest"
        targetName: "autotest"
        destinationDirectory: "bin"

        Depends { name : "Qt.testlib" }
        Depends { name: "qsttestlib" }
        Depends { name: "testdata" }

        consoleApplication: true

        files : [
            "autotest.cpp",
            "autotest.h"
        ]

        cpp.defines: base.concat([
            'PROJECTPATH="' + path + '/../' + '"'
        ])
    }

    QtApplication {
        type: [ "application", "autotest" ]
        name : "manualtest"
        targetName: "manualtest"
        destinationDirectory: "bin"

        Depends { name: "Qt.testlib" }
        Depends { name: "qsttestlib" }

        consoleApplication: true

        files : [
            "manualtest.cpp",
            "manualtest.h",
        ]

        cpp.defines: base.concat([
            'PROJECTPATH="' + path + '/../' + '"'
        ])
    }

    Product {
        type: ["resources"]
        name: "testdata"
        files: ["testdata/**/*"]

        Export {
            Depends { name: "cpp" }

            cpp.defines: base.concat([
                'TESTDATA_PATH="' + path + '/testdata/' + '"'
            ])
        }
    }

}
