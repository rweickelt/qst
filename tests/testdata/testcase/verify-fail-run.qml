import qst 1.0

TestCase {
    id : testCase
    name : "verify-fail-run"

    function run() {
        Qst.verify(false, "must not pass");
    }
}
