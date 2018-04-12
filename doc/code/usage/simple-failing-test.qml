// header-end
import qst 1.0

Testcase {
    name: "simple-failing-test"

    function run() {
        Qst.verify(false)
    }
}