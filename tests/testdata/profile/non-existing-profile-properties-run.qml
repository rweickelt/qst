import qst 1.0

Project {
    Testcase {
        id: test
        name: "non-existing-profile-properties-run"

        function run() {
            var value = profile.notExisting.child
        }
    }
}
