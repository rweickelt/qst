import qst 1.0

Project {
    name: "malformed-dimensions"

    Matrix {
        names: [
            "testcase",
        ]

        Dimension {
            animal: [
                "cat",
                "dog"
            ]
            action: [
                "studies animal rights",
                "sleeps",
                "eats"
            ]
        }

        Dimension {
            attribute: [
                "brown",
                "white"
            ]
        }
    }

    references: [
        "testcase.qml",
    ]
}
