import qbs

QtApplication {
    type: [ "application", "autotest" ]

    condition : qbs.architecture.startsWith("x86")

    property string dataDirectory: sourceDirectory

    Depends { name : "Qt.testlib" }
    Depends { name: "qsttestlib" }
    Depends { name: "qstbuildconfig" }
    Depends { name: "qst-application" }

    cpp.defines: base.concat([
        'SOURCE_DIR="' + dataDirectory + '"'
    ])

    consoleApplication: true

    Group {
        fileTagsFilter: "application"
        qbs.install: qstbuildconfig.installTests
        qbs.installDir : "bin"
    }
}
