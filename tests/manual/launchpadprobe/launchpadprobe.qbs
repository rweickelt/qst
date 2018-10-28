import qbs

QstAutoTest {
    type: [ "application" ]
    name : "manualtest-launchpadprobe"

    dataDirectory: sourceDirectory

    files : [
        "LaunchpadProbeTest.qml",
        "benchmark-response-time.qml",
        "BoardConfig.qml",
        "cc1310_launchxl_linux.json",
        "cc1310_launchxl_windows.json",
        "flash-firmware.qml",
        "pinprobe-read-write.qml",
        "reset.qml",
        "test-launchpadprobe.cpp",
        "watchdog-dut.qml",
        "watchdog.qml",
    ]
}
