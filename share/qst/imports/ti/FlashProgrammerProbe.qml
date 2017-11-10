import qst 1.0

ProcessProbe {
    id : probe

    property string installPath : "C:/Program Files (x86)/Texas Instruments/SmartRF Tools/Flash Programmer 2/bin/"
    property string target

    program : installPath + "/srfprog.exe"

    function erase() {
        probe.arguments = [
            "-t",
            probe.target,
            "--erase",
            "all"
        ];
        start();
    }

    function write() {

    }

    onFinished: {
        if (probe.exitCode != 0) {
            console.log("FlashProgrammerProbe '" + probe.name
                        + "' exited with " + probe.exitCode
                        + ": " + probe.readAllStandardError());
        }
    }

}
