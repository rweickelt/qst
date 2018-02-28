import qst 1.0
import ti 1.0

// This test suite requires 3 launchpads.
Project {
    property string dutSerial: "L400028Q" // left board
    property string probeSerial: "L20002HM" // central board
    property string uniflashInstallDir: "/opt/ti/uniflash_4.2"

    references: [
//        "flash.qml",
//        "reset.qml",
//        "verify-connection.qml",
        "pin-pair.qml"
    ]
}
