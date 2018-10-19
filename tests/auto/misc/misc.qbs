import qbs

QstAutoTest {
    name: "autotest-misc"
    files: [
        "test-misc.cpp",
        "testdata/**"
    ]
}
