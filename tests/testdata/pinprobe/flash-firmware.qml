import qst 1.0
import ti 1.0

Testcase {
    name: "flash-firmware"
    property int maxFlashTime: 15000

    UniflashProbe {
        id: board1
        serial: profile.dut.serial
        file: profile.dut.firmwarePath
        device: profile.dut.device
    }

    UniflashProbe {
        id: board2
        serial: profile.probe.serial
        file: profile.probe.firmwarePath
        device: profile.probe.device
    }

    function run() {
        board1.flash();
        board2.flash();
        board1.waitForFinished(maxFlashTime);
        board2.waitForFinished(Math.max(maxFlashTime - test.elapsedTime, 0));
        Qst.wait(666) // Otherwise the following test may not connect properly
    }
}
