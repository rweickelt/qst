import qst 1.0

Project {
    Testcase {
        name: "level-1"
        function run() {}
    }

    Testcase {
        name: "level-2"
        Depends { name: "does-not-exist" }
        function run() {}
    }
}
