import qbs

Project {
    condition : qbs.architecture === "x86_64"

QtApplication {
    type: ["application", "autotest"]
    name : "test-autotest"

    Depends { name : "Qt.testlib" }

    consoleApplication: true
    destinationDirectory: "bin"

    files : [
        "blackboxtest.cpp",
        "blackboxtest.h",
        "qsttestresults.cpp",
        "qsttestresults.h",
    ]

    Group {
        name: "testdata"
        prefix: "testdata/"
        files: ["**/*"]
        fileTags: []
    }

    cpp.defines: base.concat([
        'PROJECTPATH="' + path + '/../' + '"'
    ])
}

}
