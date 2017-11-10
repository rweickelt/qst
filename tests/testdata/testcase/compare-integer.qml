import qst 1.0

TestCase {
    id : testCase
    name : "compare-integer"

    function run() {
        Qst.compare(1701, 1701, "must pass");
        Qst.compare(47, 11, "must fail");
    }

}
