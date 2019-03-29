import qbs

QstAutoTest {
    name: "autotest-resource"
    files: [
        "depends-multiple-resources-per-name.qml",
        "depends-single-resource-per-name.qml",
        "duplicate-names.qml",
        "resources-are-returned.qml",
        "test-resource.cpp",
    ]
}
