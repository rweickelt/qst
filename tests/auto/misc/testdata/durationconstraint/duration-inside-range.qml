import QtQml 2.0
import qst 1.0


Testcase {
    name: "duration-inside-range"
    property int duration: 100
    property int deviation: 10

    signal intervalStarted
    signal intervalStopped

    DurationConstraint {
        id: constraint_signal_signal
        name: "started-by-signal-stopped-by-signal"

        minDuration: test.duration - test.deviation
        maxDuration: test.duration + test.deviation

        beginOn: intervalStarted
        endOn: intervalStopped
    }

    DurationConstraint {
        id: constraint_signal_call
        name: "started-by-signal-stopped-by-call"

        minDuration: test.duration - test.deviation
        maxDuration: test.duration + test.deviation

        beginOn: intervalStarted
    }

    DurationConstraint {
        id: constraint_call_signal
        name: "started-by-call-stopped-by-signal"

        minDuration: test.duration - test.deviation
        maxDuration: test.duration + test.deviation

        endOn: intervalStopped
    }

    DurationConstraint {
        id: constraint_call_call
        name: "started-by-call-stopped-by-call"

        minDuration: test.duration - test.deviation
        maxDuration: test.duration + test.deviation
    }

    function run() {

        intervalStarted()
        constraint_call_signal.begin()
        constraint_call_call.begin()

        Qst.compare(constraint_call_call.valid, false)
        Qst.compare(constraint_call_signal.valid, false)
        Qst.compare(constraint_signal_call.valid, false)
        Qst.compare(constraint_signal_signal.valid, false)

        Qst.wait(duration)

        // It's still before expiration
        Qst.compare(constraint_call_call.valid, false)
        Qst.compare(constraint_call_signal.valid, false)
        Qst.compare(constraint_signal_call.valid, false)
        Qst.compare(constraint_signal_signal.valid, false)

        // Start again to make sure that double-starting is not an issue
        intervalStarted()
        constraint_call_signal.begin()
        constraint_call_call.begin()

        Qst.compare(constraint_call_call.valid, false)
        Qst.compare(constraint_call_signal.valid, false)
        Qst.compare(constraint_signal_call.valid, false)
        Qst.compare(constraint_signal_signal.valid, false)

        Qst.wait(duration)

        // It's still before expiration
        Qst.compare(constraint_call_call.valid, false)
        Qst.compare(constraint_call_signal.valid, false)
        Qst.compare(constraint_signal_call.valid, false)
        Qst.compare(constraint_signal_signal.valid, false)

        // Stop all constraints before they can expire
        intervalStopped()
        constraint_call_call.end()
        constraint_signal_call.end()

        Qst.compare(constraint_call_call.valid, true)
        Qst.compare(constraint_call_signal.valid, true)
        Qst.compare(constraint_signal_call.valid, true)
        Qst.compare(constraint_signal_signal.valid, true)

        // Wait until constraints should have timed out
        Qst.wait(deviation + 2)

        Qst.compare(constraint_call_call.valid, true)
        Qst.compare(constraint_call_signal.valid, true)
        Qst.compare(constraint_signal_call.valid, true)
        Qst.compare(constraint_signal_signal.valid, true)

        // Should not have any effect because all constraints are stopped
        intervalStopped()
        constraint_call_call.end()
        constraint_signal_call.end()

        Qst.compare(constraint_call_call.valid, true)
        Qst.compare(constraint_call_signal.valid, true)
        Qst.compare(constraint_signal_call.valid, true)
        Qst.compare(constraint_signal_signal.valid, true)

        console.log("Duration: " + constraint_call_call.duration)
    }
}
