import qbs

QtApplication {
    type: [ "application", "autotest" ]

    destinationDirectory: "bin"

    Depends { name : "Qt.testlib" }
    Depends { name: "qsttestlib" }

    cpp.defines: base.concat([
        'SOURCE_DIR="' + sourceDirectory + '"'
    ])

    consoleApplication: true

    Group {
        fileTagsFilter: product.type
        qbs.install: project.installTests
        qbs.installDir : "bin"
    }
}
