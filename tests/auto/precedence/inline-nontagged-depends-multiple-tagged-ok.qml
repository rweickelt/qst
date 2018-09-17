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
            "level1"
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

    /* Multiple Depends items explicitly pointing to single job each */
    Testcase {
        property string tag
        name: "level2a"

        Depends {
            name: "level1"
            tag: "tag1"
            alias: "level1a"
        }

        Depends {
            name: "level1"
            tag: "tag2"
            alias: "level1b"
        }

        function run() {
            Qst.compare(dependencies.level1a.value, "tag1")
            Qst.compare(dependencies.level1b.value, "tag2")
        }
    }

    /* Single Depends item implicitly pointing to multiple jobs. */
    Testcase {
        name: "level2b"

        Depends {
            name: "level1"
        }

        function run() {
            var values = []
            values.push(dependencies.level1[0].value)
            values.push(dependencies.level1[1].value)
            Qst.verify(values.indexOf("tag1") > -1)
            Qst.verify(values.indexOf("tag2") > -1)
        }
    }

}
