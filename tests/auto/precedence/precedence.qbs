import qbs

QstAutoTest {
    name: "autotest-precedence"
    files: [
        "depends-illegal-name-or-alias.qml",
        "inline-cycle.qml",
        "inline-nontagged-depends-multiple-tagged-ok.qml",
        "inline-nontagged-depends-nontagged-ok.qml",
        "inline-nontagged-depends-single-tagged-ok.qml",
        "inline-tagged-depends-multiple-tagged-ok.qml",
        "inline-tagged-depends-nontagged-ok.qml",
        "inline-tagged-depends-single-matching-tagged-ok.qml",
        "inline-wrong-name.qml",
        "test-precedence.cpp",
    ]
}
