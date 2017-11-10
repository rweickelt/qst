import QtQml 2.2
import QtQml.StateMachine 1.0

StateMachine {
    id : sm
    property var targetId
    property var targetProperty
    property var values : []
    property var durations : []

    property int iteration : 0
    property int length : values.length

    function trigger() {
        sm.start();
    }

    initialState: emitting
    running : false

    State {
        id : emitting

        onEntered: {
            iteration = iteration + 1
            targetId[targetProperty] = values[iteration - 1]
            transitionTimer.interval = durations[iteration - 1];
            transitionTimer.start();
        }

        SignalTransition {
            targetState : emitting
            signal: transitionTimer.onTriggered
            guard: iteration < length
        }

        SignalTransition {
            targetState : finished
            signal: transitionTimer.onTriggered
            guard: iteration == length
        }

        Timer {
            id : transitionTimer

            running: false
            repeat: false
        }
    }

    FinalState {
        id : finished
    }

}


//    PropertyValueSequence {
//        id : resetPulse
//        targetId : resetPin
//        targetProperty : "value"

//        values    : [ 1, 0, 1 ]
//        durations : [ 0, 500, 0 ]
//    }
