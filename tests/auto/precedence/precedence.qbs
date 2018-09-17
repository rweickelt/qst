import qbs

QstAutoTest {
    name: "autotest-precedence"
    files: [
        "inline-cycle.qml",
        "inline-nontagged-depends-multiple-tagged-ok.qml",
        "inline-nontagged-depends-nontagged-ok.qml",
        "inline-nontagged-exports-illegal-name.qml",
        "inline-nontagged-depends-single-tagged-ok.qml",
        "inline-tagged-depends-multiple-tagged-ok.qml",
        "inline-tagged-depends-nontagged-ok.qml",
        "inline-tagged-depends-single-matching-tagged-ok.qml",
        "inline-wrong-name.qml",
        "test-precedence.cpp",
    ]
}
