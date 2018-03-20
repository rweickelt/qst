import qst 1.0

Testcase {
    id : testCase
    name : "arguments"

    ProcessProbe {
        id : process
        program : (Qst.hostOS === "windows") ? "cmd.exe" : "sh"
        workingDirectory: path

        readonly property var posixArguments: [
            "arguments.sh",
            "argument",
            "error"
        ]

        readonly property var windowsArguments: [
            "/C",
            "arguments.bat",
            "argument",
            "error"
        ]

        arguments : (Qst.hostOS === "windows") ? windowsArguments: posixArguments

        onStateChanged: {
            switch(state) {
            case ProcessProbe.NotRunning:
                observedExecutionPath = observedExecutionPath.concat("onStateChanged_NotRunning");
                break;
            case ProcessProbe.Starting:
                observedExecutionPath = observedExecutionPath.concat("onStateChanged_Starting");
                break;
            case ProcessProbe.Running:
                observedExecutionPath = observedExecutionPath.concat("onStateChanged_Running");
                break;
            }
        }

        onStarted: {            observedExecutionPath = observedExecutionPath.concat("onStarted");
        }

        onExitCodeChanged : {
            observedExecutionPath = observedExecutionPath.concat("onExitCodeChanged");
        }

        onFinished: {
            observedExecutionPath = observedExecutionPath.concat("onFinished");
        }
    }

    property var expectedExecutionPath : [
        "onStateChanged_Starting",
        "onStateChanged_Running",
        "onStarted",
        "onStateChanged_NotRunning",
        "onExitCodeChanged",
        "onFinished"
    ]

    property var observedExecutionPath : []


    function run() {
        process.start();
        Qst.verify(process.waitForStarted(100), "process not started");
        Qst.verify(process.waitForFinished(100), "process not finished");
        Qst.compare(process.exitCode, 47);
        Qst.compare(observedExecutionPath, expectedExecutionPath);
        var stdout = process.readAllStandardOutput().trim();
        var stderr = process.readAllStandardError().trim();
        Qst.compare(stdout, "argument");
        Qst.compare(stderr, "error");
    }


}
