import qst 1.0

Testcase {

    name: "makefile-test-simple"

    ProcessProbe {
        id: make
        program : "/usr/bin/make"
        workingDirectory: path
        arguments: [
            "-f", path + "/Makefile",
            "all"
        ]
    }

    function run() {
        make.start()
        make.waitForFinished(10)
        Qst.compare(make.exitCode, 0,
            "Build did not succeed: " + make.readAllStandardError())
    }
}