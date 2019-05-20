Project {

    Resource {
        name: "devboard"
        // Resources may contain additional information
        property string serial: "devboard-1"
    }

    Testcase {
        name: "test1a"

        Depends { name: "devboard" }

        function run() {
            // Accessing the resource
            Qst.compare(dependencies.devboard[0].serial, "devboard-1")
        }
    }

    Testcase {
        name: "test1b"

        Depends { name: "devboard" }

        function run() {
            // Accessing the resource
            Qst.compare(dependencies.devboard[0].serial, "devboard-1")
        }
    }

    Testcase {
        name: "test2"

        function run() {
            // Some work
        }
    }
}