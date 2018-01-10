import qst 1.0

Testcase {
    id : testCase
    name : "verify-pass"

    onStarted: {
        Qst.verify(true, "verify() failed in Testcase::onStarted");
    }

    function run() {
        Qst.verify(true, "verify() in Testcase::run()");
    }

    onFinished: {
        Qst.verify(result == Testcase.Unfinished, "Testcase result " + result + " should be Testcase.Unfinished in Testcase::onFinished()");
    }
}
