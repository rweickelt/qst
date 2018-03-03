import qst 1.0
import "qst_internal.js" as Internal
pragma Singleton

QstService {

    function waitUntil(expression, milliseconds) {
        var context = qmlCallerContext();

        if (typeof(expression) !== "function") {
            error("Expression must be a function", context.file, context.line);
        }

        currentTestcase.waitUntilExpression(Qt.binding(expression), milliseconds, context.file, context.line);
    }

    // Waits a given time
    function wait(milliseconds) {
        var context = qmlCallerContext();

        if (typeof(currentTestcase) === 'Undefined') {
            error("Calling Qst.wait() is only allowed in Testcase.run()", context.file, context.line);
        }

        currentTestcase.waitMilliseconds(milliseconds, context.file, context.line);
    }

    // Verifies that 'condition' is true. Otherwise the
    // test case is aborted with 'message'.
    function verify(condition, message) {
        if (condition === false) {
            if (message === undefined) {
                message = "verify() failed";
            }
            var context = qmlCallerContext();
            if (currentTestcase) {
                currentTestcase.finishAndExit(Testcase.Fail, context.file, context.line, message);
            } else {
                error(message, context.file, context.line);
            }
        }
    }

    function compare(actual, expected, message) {
        var act = Internal.formatValue(actual)
        var exp = Internal.formatValue(expected)
        var success = Internal.compare(actual, expected)

        if (success === false) {
            if (message === undefined) {
                message = "Compared values are not the same. Expected: " + exp + " Actual: " + act;
            }
            var context = qmlCallerContext();
            if (currentTestcase) {
                currentTestcase.finishAndExit(Testcase.Fail, context.file, context.line, message);
            } else {
                error(message, context.file, context.line);
            }
        }
    }
}
