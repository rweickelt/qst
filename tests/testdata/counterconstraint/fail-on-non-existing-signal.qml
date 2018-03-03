import qst 1.0

Testcase {
    name: "counting-on-signal-fail-on-overcount"

    CounterConstraint {
        id: constraint
        signal: triggered1
        maxCount: 3
    }

    function run() { }
}
