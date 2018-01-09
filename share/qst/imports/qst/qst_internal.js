// Determine what is o.
//
// Code has been taken from qmltest framework.
// http://code.qt.io/cgit/qt-labs/qtest-qml.git/
//
// Discussions and reference: http://philrathe.com/articles/equiv
// Test suites: http://philrathe.com/tests/equiv
// Author: Philippe Rathé <prathe@gmail.com>
function typeOf(o) {
    if (typeof o === "undefined") {
        return "undefined";

    // consider: typeof null === object
    } else if (o === null) {
        return "null";

    } else if (o.constructor === String) {
        return "string";

    } else if (o.constructor === Boolean) {
        return "boolean";

    } else if (o.constructor === Number) {

        if (isNaN(o)) {
            return "nan";
        } else {
            return "number";
        }
    // consider: typeof [] === object
    } else if (o instanceof Array) {
        return "array";

    // consider: typeof new Date() === object
    } else if (o instanceof Date) {
        return "date";

    // consider: /./ instanceof Object;
    //           /./ instanceof RegExp;
    //          typeof /./ === "function"; // => false in IE and Opera,
    //                                          true in FF and Safari
    } else if (o instanceof RegExp) {
        return "regexp";

    } else if (typeof o === "object") {
        if ("mapFromItem" in o && "mapToItem" in o) {
            return "declarativeitem";  // @todo improve detection of declarative items
        } else if ("x" in o && "y" in o && "z" in o) {
            return "vector3d"; // Qt3D vector
        }
        return "object";
    } else if (o instanceof Function) {
        return "function";
    } else {
        return undefined;
    }
}

// Test for equality
//
// Code has been taken from qmltest framework.
// http://code.qt.io/cgit/qt-labs/qtest-qml.git/
//
// Large parts contain sources from QUnit or http://philrathe.com
// Discussions and reference: http://philrathe.com/articles/equiv
// Test suites: http://philrathe.com/tests/equiv
// Author: Philippe Rathé <prathe@gmail.com>
function compare(act, exp) {
    var success = false;

    if (act === exp) {
        success = true; // catch the most you can
    } else if (act === null || exp === null || typeof act === "undefined" || typeof exp === "undefined") {
        success = false; // don't lose time with error prone cases
    } else {
        var typeExp = typeOf(exp), typeAct = typeOf(act)

        if (typeExp !== typeAct) {
            // allow object vs string comparison (e.g. for colors)
            // else break on different types
            if ((typeExp === "string" && typeAct === "object") || (typeExp === "object" && typeAct === "string")) {
                success = (act == exp)
            }
        } else if (typeExp === "string" || typeExp === "boolean" || typeExp === "number" ||
                   typeExp === "null" || typeExp === "undefined") {
            if (exp instanceof act.constructor || act instanceof exp.constructor) {
                // to catch short annotaion VS 'new' annotation of act declaration
                // e.g. var i = 1;
                //      var j = new Number(1);
                success = (act == exp)
            } else {
                success = (act === exp)
            }
        } else if (typeExp === "nan") {
            success = isNaN(act);
        } else if (typeExp == "number") {
            // Use act fuzzy compare if the two values are floats
            if (Math.abs(act - exp) <= 0.00001) {
                success = true
            }
        } else if (typeExp === "array") {
            success = compareArrays(act, exp)
        } else if (typeExp === "object") {
            success = compareObjects(act, exp)
        } else if (typeExp === "declarativeitem") {
            success = compareObjects(act, exp) // @todo improve comparison of declarative items
        } else if (typeExp === "vector3d") {
            success = (Math.abs(act.x - exp.x) <= 0.00001 &&
                       Math.abs(act.y - exp.y) <= 0.00001 &&
                       Math.abs(act.z - exp.z) <= 0.00001)
        } else if (typeExp === "date") {
            success = (act.valueOf() === exp.valueOf())
        } else if (typeExp === "regexp") {
            success = (act.source === exp.source && // the regex itself
                       act.global === exp.global && // and its modifers (gmi) ...
                       act.ignoreCase === exp.ignoreCase &&
                       act.multiline === exp.multiline)
        }
    }
    return success
}

// Code has been taken from qmltest framework.
// http://code.qt.io/cgit/qt-labs/qtest-qml.git/
function compareObjects(act, exp) {
    var i;
    var eq = true; // unless we can proove it
    var aProperties = [], bProperties = []; // collection of strings

    // comparing constructors is more strict than using instanceof
    if (act.constructor !== exp.constructor) {
        return false;
    }

    for (i in act) { // be strict: don't ensures hasOwnProperty and go deep
        aProperties.push(i); // collect act's properties

        if (!compare(act[i], exp[i])) {
            eq = false;
            break;
        }
    }

    for (i in exp) {
        bProperties.push(i); // collect exp's properties
    }

    // Ensures identical properties name
    return eq && compare(aProperties.sort(), bProperties.sort());

}

// Code has been taken from qmltest framework.
// http://code.qt.io/cgit/qt-labs/qtest-qml.git/
function compareArrays(actual, expected) {
    if (actual.length != expected.length) {
        return false
    }

    for (var i = 0, len = actual.length; i < len; i++) {
        if (!compare(actual[i], expected[i])) {
            return false
        }
    }

    return true
}

// Code has been taken from qmltest framework.
// http://code.qt.io/cgit/qt-labs/qtest-qml.git/
function formatValue(value) {
    if (typeof value == "object") {
        if ("x" in value && "y" in value && "z" in value) {
            return "Qt.vector3d(" + value.x + ", " +
                   value.y + ", " + value.z + ")"
        }
        try {
            return JSON.stringify(value)
        } catch (ex) {
            // stringify might fail (e.g. due to circular references)
        }
    }
    return value
}
