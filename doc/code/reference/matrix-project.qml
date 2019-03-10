import qst 1.0

Project {
    Matrix {
        Dimension {
            animal: [
                "cat",
                "dog"
            ]
        }

        Dimension {
            does: [
                "moans",
                "bites"
            ]
        }

        names: [ "tagged-test" ]
    }

    Testcase {
        name: "tagged-test"
        property string animal
        property string does

        function run() {
            Qst.info("The " + animal + " " + does + ".")
        }
    }

    Testcase {
        name: "normal-test"

        function run() {}
    }
}
