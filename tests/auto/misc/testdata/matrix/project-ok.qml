import qst 1.0

Project {
    name: "ok"

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
        }
    }

    references: [
        "testcase.qml",
    ]
}
