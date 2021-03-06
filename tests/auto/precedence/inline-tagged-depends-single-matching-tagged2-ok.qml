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

        // tagged dependency with binding
        Depends {
            name: "level1"
            tag: level2.tag
        }

        function run() {
            Qst.compare(dependencies.level1[0].tagValue, tag)
        }
    }
}
