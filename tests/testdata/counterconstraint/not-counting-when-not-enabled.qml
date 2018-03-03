import qst 1.0

Testcase {
    name: "not-counting-when-not-enabled"

    signal triggered()

    CounterConstraint {
        id: constraint
        signal: triggered
        maxCount: 3
        enabled: false
    }

    function run() {
        triggered()
        Qst.compare(constraint.count, 0)
        constraint.enabled = true
        triggered()
        Qst.compare(constraint.count, 1)
    }
}
