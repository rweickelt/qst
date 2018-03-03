import qst 1.0

Testcase {
    name: "counting-on-signal-fail-on-overcount"

    signal triggered1()
    signal triggered2()

    CounterConstraint {
        id: constraint
        signal: triggered1
        maxCount: 3
    }

    function run() {
        // Signal binding on creation
        Qst.compare(constraint.count, 0)
        triggered1()
        Qst.compare(constraint.count, 1);

        // Signal binding during run-time
        constraint.signal = triggered2
        Qst.compare(constraint.count, 1);
        triggered2()
        Qst.compare(constraint.count, 2);
        triggered1()
        Qst.compare(constraint.count, 2, "triggered1() is still connected");

        // Test overcount
        triggered2()
        triggered2() // should fail.
    }
}
