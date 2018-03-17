import qst 1.0

// Project item with in-line test cases and references.
Project {

    Testcase {
        name: "inline-1"
        function run() {
            console.log(name + " run")
        }
    }


    Testcase {
        name: "inline-2"
        function run() {
            console.log(name + " run")
        }
    }

    references: [
        "testcase1.qml",
        "testcase2.qml"
    ]

}
