import qst 1.0

Project {
    name: "malformed-dimensions"

    Matrix {
        names: [
            "testcase",
        ]

        Dimension {
            attribute: [
                "brown",
                "white"
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
