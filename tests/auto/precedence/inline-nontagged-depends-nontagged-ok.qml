import qst 1.0

Project {
    Testcase {
        name: "level3"

        Depends { name: "level1b" }
        Depends { name: "level2" }

        function run() {
            Qst.compare(dependencies.level1b.value, 47)
            Qst.compare(dependencies.level2.value, 47)
        }
    }

    Testcase {
        name: "level2"
        Depends { name: "level1a" }

        Exports {
            id: exports2
            property int value
        }

        function run() {
            Qst.compare(dependencies.level1a.value, 47)
            exports2.value = 47
        }
    }

    Testcase {
        name: "level1a"

        Exports {
            id: exports1a
            property int value
        }

        function run() {
            exports1a.value = 47
        }
    }

    Testcase {
        name: "level1b"

        Exports {
            id: exports1b
            property int value
        }

        function run() {
            exports1b.value = 47
        }
    }
}
