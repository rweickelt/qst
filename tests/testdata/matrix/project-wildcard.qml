import qst 1.0

Project {
    name: "wildcard"

    Matrix {
        testcases: [
            "*test*"
        ]

        Dimension {

        }

        Dimension {

        }
    }

    // also matched
    Testcase {
        name: "othertestcase"

        property string attribute: "lazy"
        property string animal: "elephant"

        function run() {
            Qst.info(name + ";" + attribute + ";" + animal)
        }
    }

    // not matched
    Testcase {
        name: "toastcase"

        property string attribute: "searing"
        property string animal: "monster"

        function run() {
            Qst.info(name + ";" + attribute + ";" + animal)
        }
    }

    references: [
        "testcase.qml",
    ]
}
