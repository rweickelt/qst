import qst 1.0

Component {
    id : component
    name : "child-component"

    Testcase.onStarted: {
        Qst.verify(false, "must fail");
    }
}
