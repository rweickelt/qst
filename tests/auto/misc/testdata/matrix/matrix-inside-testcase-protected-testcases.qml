import qst 1.0

Testcase {
    name: "testcase"

    property string attribute
    property string animal

    Matrix {
        Dimension {
            animal: [
                "cat",
                "dog"
            ]
        }

        Dimension {
            attribute: [
                "brown",
                "white"
            ]
        }

        // Write access to testcases property not allowed
        // here.
        names: [
            "blabla"
        ]
    }

    function run() {}
}
