import qbs

QstAutoTest {
    name: "autotest-codesnippets"

    dataDirectory: sourceDirectory + "/../../../doc/code/"

    files: [
        "test-codesnippets.cpp"
    ]

    Group {
        name: "reference"
        prefix: dataDirectory + "/reference/"

        files: [
            "depends-project-with-matrix.qml",
            "dimension-with-references.qml",
            "explicit.qml",
            "implicit.qml",
            "matrix-project.qml",
            "simple-depends.qml",
            "simple-exports.qml",
        ]
    }

    Group {
        name: "usage"
        prefix: dataDirectory + "/usage/"

        files: [
            "usage.qml",
        ]
    }
}
