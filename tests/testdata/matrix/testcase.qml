import qst 1.0

Testcase {
    name: "testcase"

    property string attribute
    property string animal

    function run() {
        Qst.info(name + ":" + attribute + ";" + animal);
    }
}
