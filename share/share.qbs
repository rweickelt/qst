import qbs

Product {
    name : "shared resources"
    type: ["copied qst resources"]

    condition : qbs.architecture.lastIndexOf("x86") === 0

    readonly property stringList qmlImportPaths : [
        sourceDirectory + "/share/qst/imports",
    ]

    Group {
        name : "qst imports"
        prefix : "qst/imports/qst/"

        files : [
            "DurationConstraint.qml",
            "PropertyValueSequence.qml",
            "Qst.qml",
            "SignalProbe.qml",
            "ValueRangeConstraint.qml",
            "qmldir",
            "qst.qmltypes",
            "qst_internal.js",
        ]

        fileTags : ["qst resources"]
        qbs.install : true
        qbs.installDir : "share/"
        qbs.installSourceBase : "."
    }

    Group {
        name : "ti-specific imports"
        prefix : "qst/imports/ti/"

        files : [
            "*.qml",
            "qmldir",
        ]

        fileTags : ["qst resources"]
        qbs.install : true
        qbs.installDir : "share/"
        qbs.installSourceBase : "."
    }
}
