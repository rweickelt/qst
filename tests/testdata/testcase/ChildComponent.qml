import qst 1.0

Component {
    id : component
    name : "child-component"

    TestCase.onStarted: {
        Qst.verify(false, "must fail");
    }
}
