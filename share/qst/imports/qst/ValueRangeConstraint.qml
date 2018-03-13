import QtQml 2.0
import qst 1.0

Component {
    property var value
    property var minValue
    property var maxValue

    property bool enabled: true
    property bool evaluateOnValidation: true
    property bool evaluateOnFinished: false
    readonly property alias valid: impl.valid

    onValueChanged: if (enabled === true) impl.validate()
    Testcase.onFinished: if ((enabled === true) && (evaluateOnFinished === true)) impl.evaluate()

    id: root

    QtObject {
        id: impl
        property bool valid: false

        function evaluate() {
            Qst.compare(valid, true,
                        "ValueRangeConstraint '"
                        + root.name
                        + "' violated. minValue: "
                        + root.minValue
                        + "; maxValue: "
                        + root.maxValue
                        + "; value: "
                        + root.value
                        + ".")
        }

        function validate() {
            console.log("validate" +value)
            Qst.verify(root.minValue <= root.maxValue,
                       "ValueRangeConstraint '"
                       + root.name
                       + "' cannot have minValue ("
                       + root.minValue
                       + ") larger than maxValue ("
                       + root.maxValue
                       + ").")
            valid = ((value >= minValue) && (value <= maxValue))

            if (evaluateOnValidation === true) {
                evaluate()
            }
        }
    }

}
