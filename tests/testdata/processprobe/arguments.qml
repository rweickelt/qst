import qst 1.0

TestCase {
    id : testCase
    name : "arguments"

    ProcessProbe {
        id : process
        program : "sh"
        workingDirectory: path

        arguments : [
            "arguments.sh",
            "argument 1",
            "error"
        ]

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

        onStarted: {
            observedExecutionPath = observedExecutionPath.concat("onStarted");
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
        Qst.compare(stdout, process.arguments[1]);
        Qst.compare(stderr, "error");
    }


}
