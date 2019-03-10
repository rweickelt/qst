import qst 1.0

Project {
    Matrix {
        Dimension {
            tag: [
                "tag1",
                "tag2"
            ]
        }

        names: [
            "level1*"
        ]
    }

    Testcase {
        name: "level1"
        property string tag

        Exports {
            id: exports
            property string value
        }

        function run() {
            exports.value = tag
        }
    }

    Testcase {
        property string tag
        name: "level2a"

        Depends {
            name: "level1"
            tag: "tag1"
        }

        function run() {
            Qst.compare(dependencies.level1[0].value, "tag1")
        }
    }

    Testcase {
        property string tag
        name: "level2b"

        Depends {
            name: "level1"
            tag: "tag2"
        }

        function run() {
            Qst.compare(dependencies.level1[0].value, "tag2")
        }
    }
}
