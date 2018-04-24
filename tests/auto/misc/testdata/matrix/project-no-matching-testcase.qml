import qst 1.0

Project {
    name: "no-matching-testcase"

    Matrix {
        testcases: [
            "nothing",
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
