import qst 1.0
import ti 1.0

Testcase {
    id : testCase
    name : "readwrite"

    PinProbe {
        id   : green
        name : "green"
        ioid : 6
        type : PinProbe.Write
        value: button1.value
        port: Xds.portFromSerial(project.serial)
    }

    PinProbe {
        id   : red
        name : "red"
        ioid : 9
        type : PinProbe.Write
        value: button1.value
        port: Xds.portFromSerial(project.serial)
    }

    PinProbe {
        id   : button1
        name : "button1"
        ioid : 13
        type : PinProbe.Read
        port: Xds.portFromSerial(project.serial)
        pullMode: PinProbe.PullUp
    }

    PinProbe {
        id   : button2
        name : "button2"
        ioid : 14
        type : PinProbe.Read
        port: Xds.portFromSerial(project.serial)
        pullMode: PinProbe.PullUp
    }

    function run() {
        console.log("Connected XDS boards: " + Xds.availableSerials());
        console.log("Starting at " + Xds.portFromSerial(project.serial));
        testCase.
        Qst.wait(100000000);
    }
}
