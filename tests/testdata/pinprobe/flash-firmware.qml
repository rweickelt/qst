import qst 1.0
import ti 1.0

Testcase {
    name: "flash-firmware"
    property int maxFlashTime: 15000

    UniflashProbe {
        id: board1
        serial: project.dut.serial
        file: project.dut.firmwarePath
        device: project.dut.device
    }

    UniflashProbe {
        id: board2
        serial: project.probe.serial
        file: project.probe.firmwarePath
        device: project.probe.device
    }

    function run() {
        console.log(Xds.availableSerials());
        board1.flash();
        board2.flash();
        board1.waitForFinished(maxFlashTime);
        board2.waitForFinished(Math.max(maxFlashTime - test.elapsedTime, 0));
        Qst.wait(666) // Otherwise the following test may not connect properly
    }
}
