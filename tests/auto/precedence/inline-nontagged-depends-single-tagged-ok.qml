import qst 1.0
import QtQuick 2.0

Project {
    Matrix {
        Dimension {
            tag: [
                "tag1",
                "tag2"
            ]
        }

        testcases: [
            "level1*"
        ]
    }

    Testcase {
        name: "level1"
        property string tag

        Exports {
            id: exports
            property string value
        }

        function run() {
            exports.value = tag
        }
    }

    Testcase {
        property string tag
        name: "level2"

        Depends {
            name: "level1"
            tag: "tag1"
        }

        function run() {
            Qst.compare(level1.value, "tag1")
        }
    }

}
