import qst 1.0

Testcase {
    name: "evaluate-on-finished"

    ValueRangeConstraint {
        id: constraint
        minValue: 1
        maxValue: 2
        evaluateOnValidation: false
        evaluateOnFinished: true
    }

    function run() {
        constraint.value = 3
        console.log("checkpoint passed")
    }
}
