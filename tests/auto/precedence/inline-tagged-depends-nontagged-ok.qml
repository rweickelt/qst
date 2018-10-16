import qst 1.0

Project {
    Matrix {
        Dimension {
            tag: [
                "tag1",
                "tag2",
                "tag3",
            ]
        }

        testcases: [
            "level2*"
        ]
    }

    Testcase {
        name: "level1"

        Exports {
            id: exports
            property int value
        }

        function run() {
            exports.value = 47
        }
    }

    Testcase {
        property string tag
        name: "level2"

        Depends { name: "level1" }

        function run() {
            Qst.compare(dependencies.level1[0].value, 47)
        }
    }

}
