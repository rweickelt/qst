import qst 1.0

Project {
    name: "overlapping-matrices"

    Matrix {
        names: [
            "test*",
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

    Matrix {
        names: [
            "test*",
        ]

        Dimension {
            animal: [
                "bat",
                "hog"
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
