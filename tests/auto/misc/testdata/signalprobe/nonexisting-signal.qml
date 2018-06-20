import qst 1.0
import QtQml 2.2 as QtQml


Testcase {
    name : "nonexisting-signal"

    SignalProbe {
        name: "probe"
        signal: test
    }

    function run() {

    }
}
