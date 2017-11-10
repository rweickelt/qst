import qst 1.0
import QtQml 2.2

TestCase {
    id : testCase
    name : "verify-fail-in-component-signal-handler"

    Timer {
        id : timer
        interval: 10

        onTriggered: {
            Qst.verify(false, "nothing must be executed hereafter");
        }
    }

    function run() {
        timer.start();
        // Test case is expected to fail during wait() because of timer,
        // resulting in an exception.
        Qst.wait(20);
        Qst.verify(false, "must be never reached");
    }
}
