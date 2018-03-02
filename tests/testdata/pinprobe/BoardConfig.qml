import QtQml 2.0
import ti 1.0

QtObject {
    property string serial
    readonly property string ttyPort: Xds.portFromSerial(serial)
    property int ioid
    property string device
    property string firmwarePath
}
