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

    Testcase {
        name: "elon_musk"
        Depends { name: "elon_musk" }
        function run() {}
    }
}
