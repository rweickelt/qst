import QtQml 2.0
import qst 1.0


Testcase {
    name: "fail-on-non-existing-end-signal"
    property int duration: 10
    property int deviation: 2

    // Signals have to be encapsulated in a component so that reference
    // validation is handled in DurationConstraint and not directly
    // by the QML engine.
    QtObject {
        id: object
        signal intervalStarted
        signal intervalStopped
    }

    DurationConstraint {
        id: constraint
        name: "duration-constraint"

        minDuration: test.duration - test.deviation
        maxDuration: test.duration + test.deviation

        beginOn: object.intervalStarted
        endOn: object.intervalStoppedd
    }

    function run() {
    }
}
