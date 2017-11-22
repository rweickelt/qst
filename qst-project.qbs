import qbs

Project {
    name: "qst-test-system"

    references : [
        "src/src.qbs",
        "examples/examples.qbs",
        "qbs/qbs-items.qbs",
        "share/share.qbs",
        "tests/tests.qbs"
    ]

    qbsSearchPaths: [
        sourceDirectory + "/qbs"
    ]

    minimumQbsVersion: "1.6"

    property bool runAutotest: false
    property bool installTests: false
}
