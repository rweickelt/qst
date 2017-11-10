import qst 1.0

TestCase {
    id : testCase
    name : "error-in-signal-handler"

    function run() { }

    TestCase.onFinished: {
        nonExistingVariable = 0;
    }
}
