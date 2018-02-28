import qst 1.0
import ti 1.0

Testcase {
    name: "flash"
    property string file: "/opt/qst/share/qst/firmware/launchpad-probe-CC1310_LAUNCHXL.elf"

    UniflashProbe {
        id: board1
        serial: project.dutSerial
        file: test.file
        device: "cc1310f128"
    }

    UniflashProbe {
        id: board2
        serial: project.probeSerial
        file: test.file
        device: "cc1310f128"
    }

    function run() {
        console.log(Xds.availableSerials());
        board1.flash();
        board2.flash();
        board1.waitForFinished(20000);
        board2.waitForFinished(Math.max(20000 - test.elapsedTime, 0));
    }
}
