import qst 1.0

Project {

    Matrix {
        Dimension {
            board: [ "board-1", "board-2" ]
        }
        Dimension {
            config: [ "debug", "release" ]
        }
        testcases: [
            "A",
            "implicit"
        ]
    }

    references: [
        "explicit.qml",
        "implicit.qml"
    ]

    Testcase {
        name: "A"
        id: a
        property string board
        property string config

        Exports {
            id: exports
            property string firmwarePath
        }

        function run() {
            exports.firmwarePath = board + "-" + config + ".elf"
        }
    }
}
