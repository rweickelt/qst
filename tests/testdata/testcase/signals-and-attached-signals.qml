import qst 1.0
import QtQml 2.2 as QtQml


Testcase {
    id : testCase
    name : "signals-and-attached-signals"

    property var observedExecutionPath : [ ]
    property var expectedExecutionPath : [
        "component.onStarted",
        "onStarted",
        "runStarted",
        "signalDuringRun",
        "runFinished",
        "component.onFinished",
        "onFinished"
    ]

    QtQml.Timer {
        id : timer
        interval: 10

        onTriggered: {
            observedExecutionPath = observedExecutionPath.concat("signalDuringRun");
        }
    }

    onStarted: {
        observedExecutionPath = observedExecutionPath.concat("onStarted");
    }

    function run() {
        observedExecutionPath = observedExecutionPath.concat("runStarted");
        timer.start();
        Qst.wait(20);
        observedExecutionPath = observedExecutionPath.concat("runFinished");
    }

    onFinished: {
        observedExecutionPath = observedExecutionPath.concat("onFinished");
        Qst.compare(observedExecutionPath, expectedExecutionPath,
                "Epected execution path: " + expectedExecutionPath
                + " observed execution path: " + observedExecutionPath);
    }

    Component {
        Testcase.onStarted : {
            observedExecutionPath = observedExecutionPath.concat("component.onStarted");
        }

        Testcase.onFinished : {
            observedExecutionPath = observedExecutionPath.concat("component.onFinished");
        }
    }
}
