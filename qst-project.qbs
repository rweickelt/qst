import qbs
import qbs.Environment
import qbs.TextFile

Project {
    name: "qst-test-system"

    references : [
        "dist/distribute.qbs",
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

    property string commit: Environment.getEnv("QST_COMMIT") || "unspecified"
    property string version: Environment.getEnv("QST_VERSION") || "local-build"
}
