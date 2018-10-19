import qst 1.0

Testcase {
    id : testCase
    name : "error-in-signal-handler"

    function run() { }

    Testcase.onFinished: {
        nonExistingVariable = 0;
    }
}
