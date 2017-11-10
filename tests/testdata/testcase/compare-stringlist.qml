import qst 1.0

TestCase {
    id : testCase
    name : "compare-stringlist"

    property var list1 : [
        "apple",
        "pear"
    ]

    property var list2 : [
        "apple",
        "pear"
    ]

    property var list3 : [
        "apple",
        "quince"
    ]


    function run() {
        Qst.compare(list1, list2, "must pass");
        Qst.compare(list1, list3, "must fail");
    }

}
