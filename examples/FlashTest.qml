import QtQml 2.2
import qst 1.0
import ti 1.0

TestCase {
    id : testCase
    name : "processTest"

    timeout : 3000 // test case is aborted after 2s

    FlashProgrammerProbe {
        id : flash1
        target : "soc(XDS-L20002HM)"

        onFinished: {
            //console.log("Flash programmer finished with " + exitCode);
        }
    }

//    ProcessProbe {
//        id : process2
//        program : "/bin/sleep"
//        arguments: "1"

//        onStarted: {
//            console.log("Process " + program + " started");
//        }

//        onFinished: {
//            console.log("Process " + program + "finished with exitcode " + exitCode);
//        }

//        onErrorOccured: {
//            console.log("Error starting process " + program + ": " + errorString);
//        }
//    }

    // Constraints check, whether the DUT behavior is ok
//    DurationConstraint {
//        id : process1_timeout_constraint
//        name : "process1_timeout_constraint"

//        from : SignalTrigger { signal : process1.started; }
//        to : SignalTrigger { signal : process1.finished; }

//        minDuration : 1
//        maxDuration : 1000
//    }

    onStarted : {
        console.log("Test case '" + name + "' started.");
        flash1.erase();
    }


    onFinished: {
        console.log("Exit code: " + flash1.exitCode);
        console.log("Test case '" + name + "' finished with result '" + result + "'.");
    }
}
