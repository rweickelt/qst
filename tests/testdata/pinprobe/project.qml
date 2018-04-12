import qst 1.0
import ti 1.0

// This test suite requires 3 launchpads.
Project {
    property string uniflashInstallDir: "/opt/ti/uniflash_4.2"
    property string qstExecutable: "/opt/qst/bin/qst"

    references: [
        "flash-firmware.qml",
        "pinprobe-read-write.qml",
        "benchmark-response-time.qml",
        "watchdog.qml"
    ]
}
