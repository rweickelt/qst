import qst 1.0

Testcase {
    name: "test-multi-build"

    MakeProbe {
        id: app
        name: "make-app"
        makefile: path + "/app.mak"
        jobs: 5
    }

    MakeProbe {
        id: lib
        name: "make-lib"
        makefile: path + "/lib.mak"
        jobs: 5
    }

    function run() {
        app.start()
        app.waitForFinished(20)
        lib.start()
        lib.waitForFinished(20)
    }

}