import qst 1.0
import "qst_internal.js" as Internal
pragma Singleton

QstService {

    function waitUntil(expression, milliseconds) {
        if (typeof(expression) !== "function") {
            var message = "expression must be a function";
            currentTestcase.finishAndExit(Testcase.Fail, currentTestcase.qmlCallerFile(), currentTestcase.qmlCallerLine(), message);
        }

        currentTestcase.waitUntilExpression(Qt.binding(expression), milliseconds, currentTestcase.qmlCallerFile(), currentTestcase.qmlCallerLine());
    }

    // Waits a given time
    function wait(milliseconds) {
        currentTestcase.waitMilliseconds(milliseconds, currentTestcase.qmlCallerFile(), currentTestcase.qmlCallerLine());
    }

    // Verifies that 'condition' is true. Otherwise the
    // test case is aborted with 'message'.
    function verify(condition, message) {
        if (condition === false) {
            if (message === undefined) {
                message = "verify() failed";
            }
            currentTestcase.finishAndExit(Testcase.Fail, currentTestcase.qmlCallerFile(), currentTestcase.qmlCallerLine(), message);
        }
    }

    function compare(actual, expected, message) {
        var act = Internal.formatValue(actual)
        var exp = Internal.formatValue(expected)
        var success = Internal.compare(actual, expected)

        if (success === false) {
            if (message === undefined) {
                message = "Compared values are not the same";
            }
            currentTestcase.finishAndExit(Testcase.Fail, currentTestcase.qmlCallerFile(), currentTestcase.qmlCallerLine(), message);
        }
    }
}