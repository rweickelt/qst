import qst 1.0

Project {
    property string prefix: {
        console.log(path + "/../../../../../doc/code/usage/")
        return path + "/../../../../../doc/code/usage/"
    }

    references: [
        prefix + "simple-passing-test.qml",
        prefix + "simple-failing-test.qml",
        prefix + "makefile-test-simple.qml",
        prefix + "test-app-build.qml",
        prefix + "test-lib-build.qml"
    ]
}
