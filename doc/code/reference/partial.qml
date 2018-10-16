import qst 1.0

Testcase {
    name: "partial"
    id: b

    property string board
    property string config

    Depends {
        name: "A"
        config: b.config
    }

    function run() {
        //var paths = dependencies.A.reduce(function(acc, value) {
        //    acc += value.firmwarePath + " "
        //    return acc
        //}, "")
        console.log(dependencies.A.firmwarePath)
        Qst.info("depends on " + dependencies.A.firmwarePath)
    }
}