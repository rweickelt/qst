import qst 1.0

Project {
    property int waitTimeMs: 100

    Resource {
        name: "r1"
        property string text: "r1"
    }

    // t1a-c have to run in series because of their resource dependency
    Testcase {
        name: "t1a"

        Depends { name: "r1" }

        function run() {
            Qst.compare(dependencies.r1[0].text, "r1")
            Qst.wait(project.waitTimeMs)
        }
    }

    Testcase {
        name: "t1b"

        Depends { name: "r1" }

        function run() {
            Qst.compare(dependencies.r1[0].text, "r1")
            Qst.wait(project.waitTimeMs)
        }
    }

    Testcase {
        name: "t1c"

        Depends { name: "r1" }

        function run() {
            Qst.compare(dependencies.r1[0].text, "r1")
            Qst.wait(project.waitTimeMs)
        }
    }

    // Should run in parallel to the other testcases and serves as a guarant
    // that parallel execution is enabled. If not, overall execution would take
    // twice as long as t2 (t1a + t1b + t1c + t2).
    Testcase {
        name: "t2"

        function run() {
            Qst.wait(3*project.waitTimeMs)
        }
    }
}
