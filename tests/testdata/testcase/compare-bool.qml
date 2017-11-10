import qst 1.0

TestCase {
    id : testCase
    name : "compare-bool"

    function run() {
        Qst.compare(true, true, "must pass");
        Qst.compare(true, false, "must fail");
    }

}
