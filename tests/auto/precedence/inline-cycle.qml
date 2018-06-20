import qst 1.0

Project {
    Testcase {
        name: "ying"
        Depends { name: "yang" }
        function run() {}
    }

    Testcase {
        name: "yang"
        Depends { name: "ying" }
        function run() {}
    }
}
