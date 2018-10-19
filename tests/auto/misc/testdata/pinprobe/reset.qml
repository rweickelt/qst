import qst 1.0
import ti 1.0

Testcase {
    name : "reset-boards"

    property string installPath: project.uniflashInstallDir
                                 + "/simplelink/gen2/bin/xds110reset"
                                 + ((Qst.hostOS === "windows") ? ".exe" : "")
    property int timeoutMs: 500

    ProcessProbe {
        id: board1
        name: "board1"
        program: installPath
        arguments: [
            "-s",
            project.dutSerial
        ]
    }

    ProcessProbe {
        id: board2
        name: "board1"
        program: installPath
        arguments: [
            "-s",
            project.probeSerial
        ]
    }

    onCreated: {
        Qst.verify(Xds.availableSerials().length >= 2, "2 launchpads need to be connected");
    }

    function run() {
        board1.start();
        board2.start();
        board1.waitForFinished(timeoutMs);
        board2.waitForFinished(Math.max(timeoutMs - test.elapsedTime, 0));
        Qst.wait(500);
    }
}
