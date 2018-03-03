import qst 1.0

Testcase {
    name: "counting-on-property"

    property int testCount1: 1
    property int testCount2: 7

    CounterConstraint {
        id: constraint
        property: testCount1
        maxCount: 47
    }

    function run() {
        // Property binding on creation
        Qst.compare(constraint.count, testCount1)
        testCount1++
        Qst.compare(constraint.count, testCount1)

        // Property binding during run-time
        constraint.property = Qt.binding(function() { return testCount2 })
        Qst.compare(constraint.count, testCount2)
        testCount2++
        Qst.compare(constraint.count, testCount2)
    }
}
