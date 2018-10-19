import qst 1.0

Testcase {
    id : testCase
    name : "verify-fail-onStarted"

    onStarted: {
        Qst.verify(false, "must not pass");
    }

    function run() {
    }
}
