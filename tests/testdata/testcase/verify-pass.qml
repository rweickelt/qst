import qst 1.0

TestCase {
    id : testCase
    name : "verify-pass"

    onStarted: {
        Qst.verify(true, "verify() failed in TestCase::onStarted");
    }

    function run() {
        Qst.verify(true, "verify() in TestCase::run()");
    }

    onFinished: {
        Qst.verify(result == TestCase.Unfinished, "TestCase result " + result + " should be TestCase.Unfinished in TestCase::onFinished()");
    }
}
