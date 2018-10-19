import qst 1.0

Testcase {
    name: "range"

    readonly property int maxInt32: 2147483647
    readonly property int minInt32: -2147483648

    readonly property double maxDouble: 171.01
    readonly property double minDouble: -4.7

    ValueRangeConstraint {
        id: int32_constraint
        minValue: minInt32 + 1
        maxValue: maxInt32 - 1
        evaluateOnValidation: false
        evaluateOnFinished: false
    }

    ValueRangeConstraint {
        id: double_constraint
        minValue: minDouble
        maxValue: maxDouble
        evaluateOnValidation: false
        evaluateOnFinished: false
    }

    function run() {
        int32_constraint.value = minInt32 + 1
        Qst.compare(int32_constraint.valid, true)
        int32_constraint.value = minInt32
        Qst.compare(int32_constraint.valid, false)

        int32_constraint.value = maxInt32 - 1
        Qst.compare(int32_constraint.valid, true)
        int32_constraint.value = maxInt32
        Qst.compare(int32_constraint.valid, false)

        double_constraint.value = minDouble +  0.1
        Qst.compare(double_constraint.valid, true)
        double_constraint.value = minDouble - 0.1
        Qst.compare(double_constraint.valid, false)

        double_constraint.value = maxDouble -  0.1
        Qst.compare(double_constraint.valid, true)
        double_constraint.value = maxDouble + 0.1
        Qst.compare(double_constraint.valid, false)

        double_constraint.evaluateOnValidation = true
        double_constraint.value = maxDouble + 0.000001
    }
}
