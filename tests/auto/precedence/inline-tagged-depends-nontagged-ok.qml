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

        property int bla

        function run() {
            Qst.compare(level1.value, 47)
        }
    }

}
