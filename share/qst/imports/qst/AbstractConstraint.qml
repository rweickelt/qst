import qst 1.0

/*
Base class for all constraints.

*/
Component {

    // Treshold value for validCount before valid is set true
    property int maxValidCount : 0

    // Treshold value for invalidCount before valid is false
    property int maxInvalidCount : 0

    // The total number of observed satisfaction events
    property int validCount : 0

    // The total number of observed violation events
    property int invalidCount : 0

    // Contains the final result of the constraint.
    property bool valid

    property bool enabled : true

    // Performs the validation each time validCount or invalidCount changes
    function validate() {
        if (validCount > maxValidCount) {
            valid = true;
        }
        if (invalidCount > maxInvalidCount) {
            valid = false;
        }
    }

    // Consumes a valid or invalid event and updates the validity counters.
    function notifyValidEvent(valid) {
        if (valid === true) {
            validCount++;
        } else if (valid === false) {
            invalidCount++;
        } else {
            throw "Expected true or false in notifyValidEvent(), but got '" + valid;
        }
        validate();
    }

    // Used to auto-determine constraint objects
    // Do not change this value
    objectName : "Constraint"
}
