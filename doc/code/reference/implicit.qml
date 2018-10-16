import qst 1.0

Testcase {
    name: "implicit"
    id: b

    property string board
    property string config

    Depends {
        name: "A"
    }

    function run() {
        var paths = dependencies.A.reduce(function(acc, value) {
            acc += value.firmwarePath + " "
            return acc
        }, "")
        Qst.info("depends on " + paths)
    }
}