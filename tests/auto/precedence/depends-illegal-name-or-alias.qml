import qst 1.0

Project {
    Testcase {
        name: "t-1"
        function run() {}
    }

    // Illegal name
    Testcase {
        name: "name-1"
        Depends { name: "t+1"; }
        function run() {}
    }

    Testcase {
        name: "name-2"
        Depends { name: "t/1"; }
        function run() {}
    }

    Testcase {
        name: "name-3"
        Depends { name: "1t"; }
        function run() {}
    }

    // Empty name
    Testcase {
        name: "empty-name"
        Depends { name: ""}
        function run() {}
    }

    // Illegal alias
    Testcase {
        name: "alias-1"
        Depends { name: "t-1"; alias: "t+1" }
        function run() {}
    }

    Testcase {
        name: "alias-2"
        Depends { name: "t-1"; alias: "t-1" }
        function run() {}
    }

    Testcase {
        name: "alias-3"
        Depends { name: "t-1"; alias: "t/1" }
        function run() {}
    }

    Testcase {
        name: "alias-4"
        Depends { name: "t-1"; alias: "1t" }
        function run() {}
    }

    // Empty alias
    Testcase {
        name: "empty-alias"
        Depends { name: "t-1"; alias: "" }
        function run() {}
    }
}
