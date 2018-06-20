import qst 1.0
import QtQml 2.2 as QtQml

// Tests trigger feature and condition
Testcase {
    name : "verify-triggered"

    property var observedExecutionPath : [ ]
    property var expectedExecutionPath : [
        "runStarted",
        "timerTriggered",
        "probeTriggered",
        "waitCompleted",
        "timerTriggered",
        "probeTriggered",
        "probeTriggered",
        "waitCompleted"
    ]

    QtQml.Timer {
        id : timer
        interval: 10

        onTriggered: {
            observedExecutionPath = observedExecutionPath.concat("timerTriggered");
        }
    }

    SignalProbe {
        id: probe1
        signal: timer.triggered

        onTriggered: {
            observedExecutionPath = observedExecutionPath.concat("probeTriggered")
        }
    }

    property bool probe2Enabled: false

    SignalProbe {
        id: probe2
        signal: timer.triggered

        condition: probe2Enabled === true
        onTriggered: {
            observedExecutionPath = observedExecutionPath.concat("probeTriggered")
        }
    }

    function run() {
        Qst.compare(probe1.count, 0);
        Qst.compare(probe2.count, 0);

        observedExecutionPath = observedExecutionPath.concat("runStarted");
        timer.start();
        Qst.wait(50);
        observedExecutionPath = observedExecutionPath.concat("waitCompleted");
        probe2Enabled = true;
        timer.start();
        Qst.wait(50);
        observedExecutionPath = observedExecutionPath.concat("waitCompleted");

        Qst.compare(probe1.count, 2);
        Qst.compare(probe2.count, 1);
        probe1.clear();
        Qst.compare(probe1.count, 0);
        Qst.compare(observedExecutionPath, expectedExecutionPath, "Wrong execution path " + observedExecutionPath.join(" "));
    }
}
