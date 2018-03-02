import qst 1.0
import QtQml 2.0

AbstractConstraint {
    property var signal
    property int count: 0
    property int maxCount: 0

    function clear() {
        count = 0
    }

    id: root

    // Private implementation
    QtObject {
        id: implementation
        function evaluate() {
            if (!enabled) {
                return
            }

            count++
            Qst.verify(count <= maxCount, "Property 'count' in CounterConstraint '"
                       + name + "' exceeded maxCount (" + maxCount + ")")
        }
    }

    Testcase.onCreated: {
        Qst.verify(typeof(signal) !== "undefined", "Property 'signal' in CounterConstraint '"
                   + name + "' is undefined.")
        Qst.verify(typeof(signal.connect) === "function", "Property 'signal' in CounterConstraint '"
                   + name + "' is not a signal.")
        signal.connect(implementation.evaluate)
    }
}
