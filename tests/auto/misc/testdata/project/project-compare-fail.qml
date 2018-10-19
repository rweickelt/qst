import qst 1.0

Project {

    property string fails: {
        Qst.compare("Äppel", "Birnen", "You should see this and get exit code 1");
    }

    references: [
        "testcase1.qml",
        "testcase2.qml"
    ]
}
