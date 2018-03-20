import qst 1.0

Testcase {
    name : "wait"

    property int waitTime : 200
    property int deviation: 5

    function run() {
        // TODO: Maybe need to allow some deviation.
        var begin = test.elapsedTime;
        Qst.compare(begin, 0, "elapsedTime must be 0 at begin");
        Qst.wait(waitTime);
        var time = test.elapsedTime - begin;
        Qst.verify(((time >= (waitTime - deviation)) && (time <= (waitTime + deviation))), "expected: " + waitTime + "ms; actual: " + time + "ms");
    }
}
