import qst 1.0
import ti 1.0

Project {
    property string serial: {
        var serials = Xds.availableSerials();
        console.log(serials);
        Qst.verify(serials.length > 0, "No boards connected");
        return serials[0];
    }

    property string platform: Qst.hostOS;

    references: [
        "readwrite.qml",
    ]
}
