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
        "qbs"
    ]

    minimumQbsVersion: "1.6"
}
