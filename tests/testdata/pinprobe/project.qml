import qst 1.0
import ti 1.0

// This test suite requires 3 launchpads.
Project {
    property string uniflashInstallDir: "/opt/ti/uniflash_4.2"
    property string qstExecutable: "/opt/qst/bin/qst"

    property var dut: BoardConfig {
        serial: "L400028Q" // left board
        device: "cc1310f128"
        firmwarePath: "/opt/qst/share/qst/firmware/launchpad-probe-CC1310_LAUNCHXL.elf"
        ioid: 13
    }

    property var probe: BoardConfig {
        serial: "L20002HM" // central board
        device: "cc1310f128"
        firmwarePath: "/opt/qst/share/qst/firmware/launchpad-probe-CC1310_LAUNCHXL.elf"
        ioid: 23
    }

    references: [
        "flash-firmware.qml",
        "pinprobe-read-write.qml",
        "watchdog.qml"
    ]
}
