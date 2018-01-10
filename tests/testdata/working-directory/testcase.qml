import qst 1.0

Testcase {
    id : testCase
    name : "testcase"

    property string projectWorkingDirectory : project.workingDirectory

    // Verify that these properties are set as early as possible.
    onCreated: {
        Qst.verify(testCase.workingDirectory.length > 0, "testCase.workingDirectory is not set");
        Qst.verify(project.workingDirectory.length > 0, "project.workingDirectory is not set")
        Qst.compare(projectWorkingDirectory, project.workingDirectory,
                "project.workingDirectory was not set during creation");
        Qst.compare(testCase.workingDirectory, projectWorkingDirectory + "/" + testCase.name,
                "testCase.workingDirectory is wrong.");
    }

    function run() {}

}
