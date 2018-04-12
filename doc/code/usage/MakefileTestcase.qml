import qst 1.0

Testcase {

    property string makefile : path + "/Makefile"
    property string target: "all"
    property int timeout : 10

    ProcessProbe {
        id: make
        program : (Qst.hostOs === "windows") ? "gmake.exe" : "make"
        workingDirectory: path
        arguments: [
            "-f", makefile,
            target
        ]
    }

    function run() {
        make.start()
        make.waitForFinished(timeout)
        Qst.compare(make.exitCode, 0,
            "Build did not succeed: " + make.readAllStandardError())
    }
}