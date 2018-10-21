import qst 1.0
import ti 1.0

// Internal helper file for watchdog.qml
LaunchpadProbeTest {
    name: "watchdog-dut"

    PinProbe {
        name: "arbitrary-pin"
        ioid: 1
        type: PinProbe.Read
        pullMode: PinProbe.PullDown
        port: Xds.portFromSerial(profile.dut.serial) // left board
    }

    function run() {
        Qst.wait(2000)
    }
}
