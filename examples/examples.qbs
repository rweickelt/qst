import qbs

Product {
    name : "usage examples"

    readonly property stringList qmlImportPaths : [
        path + "/../share/qst/imports",
    ]

    files : [
        "*.qml",
    ]
}
