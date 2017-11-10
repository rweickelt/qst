import qbs

Product {
    name : "shared-types"

    Group {
        name: "protocols"
        prefix: "protocols/"
        files: [
            "pin.h",
            "roc.h",
            "stp.h",
        ]
    }

    Export {
        Depends { name : "cpp" }
        cpp.includePaths : path
    }
}
