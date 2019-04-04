import qbs

QstAutoTest {
    name: "autotest-resource"
    files: [
        "depends-multiple-resources-per-name.qml",
        "depends-single-resource-per-name.qml",
        "duplicate-names.qml",
        "parallel-execution-exclusive-access.qml",
        "resources-are-returned.qml",
        "test-resource.cpp",
    ]
}
