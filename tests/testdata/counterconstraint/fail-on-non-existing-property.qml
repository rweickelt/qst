import qst 1.0

Testcase {
    name: "fail-on-non-existing-property"

    CounterConstraint {
        id: constraint
        property: testCount1
        maxCount: 47
    }

    function run() { }
}
