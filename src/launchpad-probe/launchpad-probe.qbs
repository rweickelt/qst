import qbs
import qbs.FileInfo

Project {
    condition : qbs.architecture === "arm"

    Product {
        name: "launchpad-probe"
        type: "application"

        simplelink.board.type: "CC1310_LAUNCHXL"

        targetName: "launchpad-probe-" + simplelink.board.type + ".elf"

        Depends { name: "cpp" }
        Depends { name: "simplelink"; submodules: [ "core", "drivers", "tirtos", "board" ] }
        Depends { name: "shared-types" }

        cpp.driverFlags: [
            "-mcpu=cortex-m3",
            "-mthumb",
        ]

        cpp.cxxLanguageVersion: "c++14"

        qbs.optimization: "small"

        files: [
            "arraylist.h",
            "interruptlock.h",
            "mailbox.h",
            "main.cpp",
            "messagebuffer.cpp",
            "messagebuffer.h",
            "pintargetobject.cpp",
            "pintargetobject.h",
            "roctargetcontroller.cpp",
            "roctargetcontroller.h",
            "roctargetobject.cpp",
            "roctargetobject.h",
            "serialinterface.h",
            "serialinterface.cpp",
            "project.cfg",
            "shareddata.h",
            "sharedpointer.h",
        ]

        Group {
            fileTagsFilter: "application"
            qbs.install: true
            qbs.installDir: "share/qst/firmware"
        }
    }
}

