import qst 1.0

Testcase {
    id : testCase
    name : "onErrorOccured"

    property bool errorHasOccured : false;

    ProcessProbe {
        id : process
        program : "some-random-invalid-command"
        workingDirectory: path

        onErrorOccured : {
            Qst.compare(error, ProcessProbe.FailedToStart);
            errorHasOccured = true;
        }
    }

    function run() {
        process.start();
        Qst.wait(50);
        Qst.verify(errorHasOccured === true);
    }


}
