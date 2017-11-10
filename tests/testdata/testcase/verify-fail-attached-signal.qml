import qst 1.0

TestCase {
    id : testCase
    name : "verify-fail-attached-signal"

    function run() { }

    Component {
        TestCase.onStarted : {
            verify(false, "must fail")
        }
    }
}
