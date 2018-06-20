import qst 1.0

Project {
    Testcase {
        name: "level-1"
        function run() {}

        Exports {
            property string someProperty: "someValue"
        }
    }

    Testcase {
        name: "level-2a"
        Depends { name: "level-1" }
        function run() {}
    }

    Testcase {
        name: "level-2b"
        Depends { name: "level-1"; alias: "level1" }
        function run() {}
    }

    Testcase {
        name: "level-2c"
        Depends { name: "level-1"; alias: "level+1" }
        function run() {}
    }

    Testcase {
        name: "level-2d"
        Depends { name: "level-1"; alias: "Level1" }
        function run() {}
    }
}
