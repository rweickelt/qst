import qst 1.0

/*
Make sure that multiple resources with the same name identifier are
correctly assigned.
*/
Project {
    Resource {
        name: "r1"
        property string text: "r1a"
    }

    Resource {
        name: "r1"
        property string text: "r1b"
    }

    Resource {
        name: "r1"
        property string text: "r1c"
    }

    Testcase {
        name: "t"

        Depends {
            name: "r1"
            count: 2
        }

        function run() {

            var possible = [
                "r1a",
                "r1b",
                "r1c"
            ]

            var actual = [
                dependencies.r1[0].text,
                dependencies.r1[1].text
            ]

            Qst.verify(dependencies.r1.length === 2)
            Qst.verify(actual[0] !== actual[1])
            Qst.verify(possible.indexOf(actual[0]) > -1)
            Qst.verify(possible.indexOf(actual[1]) > -1)
        }
    }
}
