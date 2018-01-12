import qst 1.0

Testcase {
    id : testCase
    name : "readwrite"

    property string port: "/dev/ttyACM3"

    PinProbe {
        id   : green
        name : "green"
        ioid : 7
        type : PinProbe.Write
        value: button.value
        port: testCase.port
    }

    PinProbe {
        id   : red
        name : "red"
        ioid : 6
        type : PinProbe.Write
        port: testCase.port
    }

    PinProbe {
        id   : button
        name : "button"
        ioid : 14
        type : PinProbe.Read
        port: testCase.port
        pullMode: PinProbe.PullUp
    }

    function run() {
        console.info("starting");
        Qst.wait(100000000);
    }
}
