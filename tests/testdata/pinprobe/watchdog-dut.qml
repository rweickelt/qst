import qst 1.0
import ti 1.0

// Internal helper file for watchdog.qml
Testcase {
    name: "watchdog-dut"

    PinProbe {
        name: "arbitrary-pin"
        ioid: 1
        type: PinProbe.Read
        pullMode: PinProbe.PullDown
        port: Xds.portFromSerial("L400028Q") // left board
    }

    function run() {
        Qst.wait(2000)
    }
}
