import qst 1.0

Project {
    property string value: profile.value

    Testcase {
        id: test
        name: "existing-profile-properties"

        property string value: profile.value

        function run() {
            console.log("location:" + profile.location)
            console.log("variant:" + profile.variant)
            Qst.compare(project.value, "value")
            Qst.compare(test.value, "value")
        }
    }
}
