import qst 1.0

Project {
    Matrix {
        Dimension {
            tag: [
                "tag1",
                "tag2",
                "tag3"
            ]
        }

        testcases: [
            "*"
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
        name: "level2"

        Depends {
            name: "level1"
            tag: [
                "tag1",
                "tag2"
            ]
            alias: "level1"
        }

        function run() {
            var values = []
            values.push(dependencies.level1[0].value)
            values.push(dependencies.level1[1].value)
            Qst.compare(dependencies.level1.length, 2)
            Qst.verify(values.indexOf("tag1") > -1)
            Qst.verify(values.indexOf("tag2") > -1)
        }
    }
}
