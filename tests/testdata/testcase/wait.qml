import qst 1.0

Testcase {
    name : "wait"

    property int waitTime : 50

    function run() {
        // TODO: Maybe need to allow some deviation.
        var begin = elapsedTime;
        Qst.compare(begin, 0, "elapsedTime must be 0 at begin");
        Qst.wait(waitTime);
        var end = elapsedTime;
        Qst.compare(end, waitTime, "elapsedTime must be " + waitTime + " at end");
    }
}
