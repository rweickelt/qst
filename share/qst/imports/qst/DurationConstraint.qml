import QtQml.StateMachine 1.0
import qst 1.0

Component {
    property var beginOn
    property var endOn

    readonly property alias duration : impl.duration
    property double minDuration : 0
    property double maxDuration : 0

    property bool enabled: true
    property bool evaluateOnValidation: true
    property bool evaluateOnFinished: false
    readonly property alias timedOut: impl.timedOut
    readonly property alias valid: impl.valid

    function begin() { impl.handleBeginSignal() }
    function end() { impl.handleEndSignal() }

    id: root
    onBeginOnChanged: impl.setBeginSignal(beginOn)
    onEndOnChanged: impl.setEndSignal(endOn)
    Testcase.onFinished: if ((enabled === true) && (evaluateOnFinished === true)) impl.evaluate()

    StateMachine {
        id: impl
        property double duration : 0
        property bool started: false
        property bool timedOut: false
        property bool valid: false

        property var beginSignal
        property var endSignal
        property var backupBeginSignal
        property var backupEndSignal

        signal measurementFinished()
        signal measurementStarted()

        running: root.enabled
        initialState: idleState


        function setBeginSignal(newSignal) {
            if (typeof(backupBeginSignal) !== "undefined") {
                backupBeginSignal.disconnect(handleBeginSignal)
            }

            Qst.verify(typeof(newSignal) !== "undefined", "Property 'beginOn' in DurationConstraint '"
                       + name + "' is undefined.")
            Qst.verify(typeof(newSignal.connect) === "function", "Property 'beginOn' in DurationConstraint '"
                       + name + "' is not a signal.")

            newSignal.connect(handleBeginSignal)
            backupBeginSignal = newSignal
        }

        function setEndSignal(newSignal) {
            if (typeof(backupEndSignal) !== "undefined") {
                backupEndSignal.disconnect(handleEndSignal)
            }

            Qst.verify(typeof(newSignal) !== "undefined", "Property 'endOn' in DurationConstraint '"
                       + name + "' is undefined.")
            Qst.verify(typeof(newSignal.connect) === "function", "Property 'endOn' in DurationConstraint '"
                       + name + "' is not a signal.")

            newSignal.connect(handleEndSignal)
            backupEndSignal = newSignal
        }

        function handleBeginSignal() {
            measurementStarted()
        }

        function handleEndSignal() {
            measurementFinished()
        }

        function evaluate() {
            Qst.compare(valid, true,
                        "DurationConstraint '"
                        + root.name
                        + "' violated. minDuration: "
                        + root.minDuration
                        + "ms; maxDuration: "
                        + root.maxDuration
                        + "ms; duration: "
                        + root.duration
                        + "ms; timedOut: "
                        + impl.timedOut
                        + ".")
        }

        function validate() {
            Qst.verify(root.minDuration <= root.maxDuration,
                       "DurationConstraint '"
                       + root.name
                       + "' cannot have minDuration ("
                       + root.minDuration
                       + "ms) larger than maxDuration ("
                       + root.maxDuration
                       + "ms).")
            return (root.duration >= root.minDuration) && (root.duration <= root.maxDuration)
        }

        State {
            id: idleState

            SignalTransition {
                targetState : measuringState
                signal : impl.measurementStarted
            }
        }

        State {
            id: measuringState
            property double beginTime: 0

            onEntered: {
                impl.timedOut = false
                beginTime = test.elapsedTime
            }

            onExited: {
                impl.duration = test.elapsedTime - beginTime
            }

            SignalTransition {
                targetState: measuringState
                signal: impl.measurementStarted
            }

            SignalTransition {
                targetState: validatingState
                signal: impl.measurementFinished
            }

            TimeoutTransition {
                targetState: timeoutState
                timeout: root.maxDuration + 1
            }
        }

        State {
            id: validatingState

            onEntered: {
                impl.valid = impl.validate()
                if (root.evaluateOnValidation === true) {
                    impl.evaluate()
                }
            }

            TimeoutTransition {
                targetState: idleState
                timeout: 0
            }
        }

        State {
            id: timeoutState

            onEntered: {
                impl.duration = root.maxDuration + 1
                impl.timedOut = true
                impl.valid = impl.validate()
                if (root.evaluateOnValidation === true) {
                    impl.evaluate()
                }
            }

            TimeoutTransition {
                targetState: idleState
                timeout: 0
            }
        }
    }
}
