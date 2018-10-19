import QtQml 2.0
import qst 1.0


Testcase {
    name: "duration-below-min"
    property int duration: 10
    property int deviation: 2

    signal intervalStarted
    signal intervalStopped

    DurationConstraint {
        id: constraint
        name: "duration-constraint"

        minDuration: test.duration - test.deviation
        maxDuration: test.duration + test.deviation

        beginOn: intervalStarted
        endOn: intervalStopped
    }

    function run() {
        intervalStarted()
        Qst.wait(3)
        intervalStopped()
    }
}
