import qst 1.0

Project {
    name: "incompatible-properties"

    Matrix {
        names: [
            "testcase",
        ]

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
            doesntexist: "no"
        }
    }

    references: [
        "testcase.qml",
    ]
}
