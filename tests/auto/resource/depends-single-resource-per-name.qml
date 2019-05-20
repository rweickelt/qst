import qst 1.0

Project {
    Resource {
        name: "r1"
        property string text: "r1"
    }

    Resource {
        name: "r2"
        property string text: "r2"
    }

    Testcase {
        name: "t"

        Depends {
            name: "r1"
        }
        Depends {
            name: "r2"
            alias: "rA"
        }

        function run() {
            Qst.verify(dependencies.r1[0])
            Qst.compare(dependencies.r1[0].text, "r1")
            Qst.verify(dependencies.rA[0])
            Qst.verify(dependencies.rA[0].text, "r2")
        }
    }
}
