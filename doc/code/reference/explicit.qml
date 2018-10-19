import qst 1.0

Testcase {
    name: "explicit"
    id: b

    Depends {
        name: "A"
        board: [
            "board-1",
            "board-2"
        ]
        config: "debug"
    }

    function run() {
        var paths = dependencies.A.reduce(function(acc, value) {
            acc += value.firmwarePath + " "
            return acc
        }, "")
        Qst.info("depends on " + paths)
    }
}