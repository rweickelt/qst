import QtQml 2.0
import qst 1.0

Component {
    id: root
    readonly property alias count: impl.count
    property bool enabled: true
    property int maxCount: 0
    property int property: 0
    property var signal
    readonly property alias valid: impl.valid
    property bool validateImmediately: true

    function clear() { impl.clear(); }

    onPropertyChanged: impl.handlePropertyUpdate()
    onSignalChanged: impl.setSignal(signal)
    Testcase.onFinished: { if ((enabled === true) && (validateImmediately === false)) impl.validate() }

    // Private implementation
    QtObject {
        id: impl
        property int count: 0
        property var backupSignal
        property bool valid: (count <= maxCount)

        function handlePropertyUpdate() {
            if (enabled === false) {
                return
            }
            count = property
            if (validateImmediately === true) {
                validate()
            }

        }

        function handleSignal() {
            if (enabled === false) {
                return
            }
            count++
            if (validateImmediately === true) {
                validate()
            }
        }

        function clear() {
            count = 0
        }

        function setSignal(newSignal) {
            if (typeof(backupSignal) !== "undefined") {
                backupSignal.disconnect(handleSignal)
            }

            Qst.verify(typeof(newSignal) !== "undefined", "Property 'signal' in CounterConstraint '"
                       + name + "' is undefined.")
            Qst.verify(typeof(newSignal.connect) === "function", "Property 'signal' in CounterConstraint '"
                       + name + "' is not a signal.")

            newSignal.connect(handleSignal)
            backupSignal = newSignal
        }

        function validate() {
            Qst.verify(count <= maxCount, "Property 'count' (" + count + ") in CounterConstraint '"
                       + name + "' exceeded maxCount (" + maxCount + ")")
        }
    }
}
