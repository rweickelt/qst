import qst 1.0

Testcase {
    id : testCase
    name : "compare-string"

    function run() {
        Qst.compare("apples", "apples", "must pass");
        Qst.compare("apples", "pears", "must fail");
    }

}
