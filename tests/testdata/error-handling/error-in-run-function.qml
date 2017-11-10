import qst 1.0

TestCase {
    id : testCase
    name : "error-in-run-function"

    function run() {
        nonExistingVariable = 0;
    }
}
