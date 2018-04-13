import qst 1.0

ProcessProbe {
    property string makefile : path + "/Makefile"
    property string target: "all"
    property int jobs: 1

    program : (Qst.hostOs === "windows") ? "gmake.exe" : "make"
    arguments: [
        "-f", makefile,
        "-j", jobs,
        target
    ]
    workingDirectory: path

    // Explicitly defined by ProcessProbe
    onFinished: {
        Qst.compare(exitCode, 0, readAllStandardError())
    }

    // Implicitly defined property change signal
    onJobsChanged: {
        Qst.verify(jobs <= 4, "The maximum number of jobs is 4.")
    }

    // Attached by Testcase
    Testcase.onFinished: {
        if (state === ProcessProbe.Running) {
            terminate()
            Qst.verify(false, "Make was still running.")
        }
    }

}