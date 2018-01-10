import qst 1.0
import ti 1.0

Testcase {
    id : testCase
    name : "EmptyTest"

    ProcessProbe {
        name : "delay-process"
        id : probe1
        program : "sh"
        arguments: "delay1s.sh"
        workingDirectory: path

        onFinished: {
            verify(exitCode === 1, "Probe exited with wrong code");
        }
    }

    DurationConstraint {
        name : "probe1 finished well"
        from : SignalTrigger { signal : probe1.started; }
        to : SignalTrigger { signal : probe1.finished; condition: probe1.exitCode === 1; }
        minDuration: 0
        maxDuration: 600
    }

    ProcessProbe {
        id : probe2
        name : "ls-process"
        program : "ls"

        onFinished: {
            verify(exitCode === 0, "Probe exited with wrong code");
        }
    }

    function run() {

    }

    function test_textFile() {
        var file = Qst.createObject("TextFile", { "filePath" : "/tmp/TestFile.xml" });
        file.write("Hello World");
    }
}
