import qst 1.0

Project {
    Matrix {
        Dimension {
            tag: [
                "tag1",
                "tag2"
            ]
        }

        testcases: [
            "*",
        ]
    }

    Testcase {
        property string tag
        name: "level1"

        Exports {
            id: exports
            property string tagValue
        }

        function run() {
            exports.tagValue = tag
        }
    }

    Testcase {
        id: level2
        property string tag
        name: "level2"

        Depends { name: "level1" }

        function run() {
            Qst.info("tag: " + tag + ", level1.tagValue: " + level1.tagValue)
            Qst.compare(level1.tagValue, tag)
        }
    }
}
