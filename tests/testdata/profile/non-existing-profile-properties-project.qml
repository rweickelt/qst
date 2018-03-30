import qst 1.0

Project {
    property string value: profile.notExisting

    Testcase {
        id: test
        name: "non-existing-profile-properties-project"

        function run() {
        }
    }
}
