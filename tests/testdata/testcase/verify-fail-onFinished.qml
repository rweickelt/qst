import qst 1.0

Testcase {
    id : testCase
    name : "verify-fail-onFinished"

    function run() { }

    onFinished: {
        Qst.verify(false, "must not pass");
    }
}
