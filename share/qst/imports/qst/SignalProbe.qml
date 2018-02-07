import qst 1.0

Component {
    id: trigger

    property var signal
    property bool condition: true
    property int count: 0

    signal triggered()

    function clear() {
        count = 0;
    }

    function evaluate() {
        if (condition) {
            count++;
            triggered();
        }
    }

    Testcase.onCreated: {
        Qst.verify(typeof(trigger.signal) !== 'undefined', "Property 'signal' in SignalProbe '"
                   + trigger.name + "' is undefined.");
        Qst.verify(typeof(trigger.signal.connect) === 'function', "Property 'signal' in SignalProbe '"
                   + trigger.name + "' is not a signal.");
        signal.connect(evaluate);
    }
}
