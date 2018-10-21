import qbs

QstAutoTest {
    type: [ "application" ]
    name : "manualtest-launchpadprobe"

    dataDirectory: sourceDirectory

    files : [
        "LaunchpadProbeTest.qml",
        "benchmark-response-time.qml",
        "BoardConfig.qml",
        "cc1310_launchxl.json",
        "flash-firmware.qml",
        "pinprobe-read-write.qml",
        "reset.qml",
        "test-launchpadprobe.cpp",
        "test-launchpadprobe.h",
        "watchdog-dut.qml",
        "watchdog.qml",
    ]
}
