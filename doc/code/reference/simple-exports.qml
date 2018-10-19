import qst 1.0

Project {

    Testcase {
        name: "A"

        Exports {
            id: exports
            property string result
        }

        function run() {
            exports.result = "excellent"
        }
    }

    Testcase {
        name: "B"

        Depends {
            name: "A"
        }

        function run() {
            Qst.info("A was " + dependencies.A[0].result)
        }
    }
}
