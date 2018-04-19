import qst 1.0

Project {
    name: "matrix-project"

    Matrix {
        testcases: [
            "sometest"
        ]

        Dimension {

        }

        Dimension {

        }

        Dimension {

        }
    }

    Testcase {
        name: "sometest"

        property string animal
        property string attribute
        property string op

        function run() {
            Qst.info("The " + attribute + " " + animal + " " + op + ".");
        }
    }
}
