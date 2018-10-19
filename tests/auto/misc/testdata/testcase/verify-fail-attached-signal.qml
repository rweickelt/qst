import qst 1.0

Testcase {
    id : testCase
    name : "verify-fail-attached-signal"

    function run() { }

    Component {
        Testcase.onStarted : {
            verify(false, "must fail")
        }
    }
}
