import qst 1.0

Project {

    // @snip
    Matrix {
        id: matrix

        testcases: "*"

        property var animals: {
            var values = []
            values.push("cat")
            values.push("dog")
            return values
        }

        property string color: "white"

        Dimension {
            animal: matrix.animals
        }

        Dimension {
            attribute: [
                "brown",
                matrix.color
            ]
        }
    }
    // @snap

    Testcase {
        name: "testcase"

        property string animal: "sloth"
        property string attribute: "sleepy"

        function run() {}
    }
}