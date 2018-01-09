import qst 1.0
import "qst_internal.js" as Internal
pragma Singleton

QstService {

    function waitUntil(expression, milliseconds) {
        if (typeof(expression) !== "function") {
            var message = "expression must be a function";
            currentTestCase.finishAndExit(TestCase.Fail, currentTestCase.qmlCallerFile(), currentTestCase.qmlCallerLine(), message);
        }

        currentTestCase.waitUntilExpression(Qt.binding(expression), milliseconds, currentTestCase.qmlCallerFile(), currentTestCase.qmlCallerLine());
    }

    // Waits a given time
    function wait(milliseconds) {
        currentTestCase.waitMilliseconds(milliseconds, currentTestCase.qmlCallerFile(), currentTestCase.qmlCallerLine());
    }

    // Verifies that 'condition' is true. Otherwise the
    // test case is aborted with 'message'.
    function verify(condition, message) {
        if (condition === false) {
            if (message === undefined) {
                message = "verify() failed";
            }
            currentTestCase.finishAndExit(TestCase.Fail, currentTestCase.qmlCallerFile(), currentTestCase.qmlCallerLine(), message);
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
            currentTestCase.finishAndExit(TestCase.Fail, currentTestCase.qmlCallerFile(), currentTestCase.qmlCallerLine(), message);
        }
    }
}
