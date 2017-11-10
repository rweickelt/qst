import QtQuick 2.0
import QtQml.StateMachine 1.0

Item {
    property var signal
    property bool condition : true

    signal fired()

    function evaluate() {
        if (condition)
        {
            fired();
        }
    }

    Component.onCompleted: {
        signal.connect(evaluate);
    }
}
