import QtQml.StateMachine 1.0
import qst 1.0

AbstractConstraint {

    property string name : "undefined"
    property double minDuration : 0
    property double maxDuration : 0
    property double duration : 0
    property double maxDeviation : 0

    property SignalTrigger from
    property SignalTrigger to

    id : constraint

    property StateMachine stateMachine :
        StateMachine {

            id : sm

            /* Internal values */
            property double beginTime : 0
            property double endTime : 0
            property double measuredDuration : 0

            // Helper signals
            signal measurementStarted()
            signal measurementStopped()

            // Connectors for 'from' and 'to' in the constraint
            function startMeasurement() { sm.measurementStarted(); }
            function stopMeasurement() { sm.measurementStopped(); }

            initialState : init
            running : constraint.enabled

            State {
                id : init

                // setup
                onEntered: {
                    console.assert(from, "property 'from' is not defined");
                    console.assert(to, "property 'to' is not defined");
                    if (from)
                        from.onFired.connect(sm.startMeasurement);
                    if (to)
                        to.onFired.connect(sm.stopMeasurement);

                    console.assert((duration >= 0),
                                   "Error: duration must be greater or equal 0");
                    console.assert((maxDeviation >= 0),
                                   "Error: deviation must be greater or equal 0");
                    console.assert((minDuration >= 0),
                                   "Error: minDuration must be greater or equal 0");
                    console.assert((maxDuration >= 0),
                                   "Error: maxDuration must be greater or equal 0");

                    console.assert(!((duration != 0) && (minDuration != 0)),
                                   "Error: duration and minDuration must not be set at the same time.");
                    console.assert(!((duration != 0) && (maxDuration != 0)),
                                   "Error: duration and maxDuration must not be set at the same time.");
                    console.assert((minDuration <= maxDuration),
                                   "Error: minDuration must be less or equal maxDuration");

                    if (duration != 0)
                    {
                        minDuration = duration - maxDeviation;
                        maxDuration = duration + maxDeviation;
                    }
                }

                TimeoutTransition {
                    targetState: idle
                    timeout: 0
                }
            }

            State {
                id : idle

                SignalTransition {
                    targetState : measuring
                    signal : sm.measurementStarted
                }

//                SignalTransition {
//                    targetState : measuring
//                    signal : constraint.froms
//                    guard : constraint.froms === true
//                }
            }

            State {
                id : measuring

                onEntered : {
                    sm.beginTime = Date.now();
                }

                onExited: {
                    sm.endTime = Date.now();
                    sm.measuredDuration = sm.endTime - sm.beginTime;

                    if (sm.measuredDuration < minDuration) {
                        console.log(constraint.name
                                    + ": fail (duration: " + sm.measuredDuration
                                    + ", minDuration: " + minDuration);
                        notifyValidEvent(false);

                    } else if (sm.measuredDuration > maxDuration) {
                        console.log(constraint.name
                                    + ": fail (duration: " + sm.measuredDuration
                                    + ", maxDuration: " + maxDuration);
                        notifyValidEvent(false);

                    } else {
                        notifyValidEvent(true);
                    }

                }

                SignalTransition {
                    targetState : idle
                    signal : sm.measurementStopped
                }
            }
        }
}
