import qst 1.0

Project {

    Testcase {
        name: "A"

        function run() {}
    }

    // Has to wait until A completes.
    Testcase {
        name: "B"

        Depends { name: "A" }

        function run() {}
    }
}