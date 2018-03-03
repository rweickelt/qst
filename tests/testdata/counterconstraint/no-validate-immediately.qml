import qst 1.0

Testcase {
    name: "no-validate-immediately"

    signal triggered()

    CounterConstraint {
        id: constraint
        signal: triggered
        maxCount: 0
        validateImmediately: false
    }

    function run() {
        Qst.compare(constraint.valid, true)
        triggered()
        Qst.compare(constraint.valid, false)
        triggered()
        Qst.compare(constraint.count, 2)
        // If we have passed this point, the error message will print count 2. That's what
        // we want.
        // If we haven't passed and the first triggered() already made the test fail, then
        // the error message will report count 1.
    }
}
