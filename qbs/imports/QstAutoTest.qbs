import qbs

QtApplication {
    type: [ "application", "autotest" ]

    condition : { return qbs.architecture.startsWith("x86") }

    property string dataDirectory: sourceDirectory

    destinationDirectory: "bin"

    Depends { name : "Qt.testlib" }
    Depends { name: "qsttestlib" }

    cpp.defines: base.concat([
        'SOURCE_DIR="' + dataDirectory + '"'
    ])

    consoleApplication: true

    Group {
        fileTagsFilter: product.type
        qbs.install: project.installTests
        qbs.installDir : "bin"
    }
}
