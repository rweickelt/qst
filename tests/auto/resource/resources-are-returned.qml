import qst 1.0

Project {
    Resource {
        name: "r1"
        property string text: "r1"
    }

    Testcase {
        name: "t1"

        Depends {
            name: "r1"
        }

        function run() {
            Qst.compare(dependencies.r1[0].text, "r1")
        }
    }

    Testcase {
        name: "t2"

        Depends { name: "t1" }
        Depends { name: "r1" }

        function run() {
            Qst.compare(dependencies.r1[0].text, "r1")
        }
    }
}
