import qst 1.0

Project {

    property string fails: {
        Qst.wait(1); // Should result in error message and exit code 1
    }

    references: [
        "testcase1.qml",
        "testcase2.qml"
    ]
}
