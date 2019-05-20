import qbs
import qbs.FileInfo

Project {
    condition : qbs.architecture === "arm"

    Product {
        name: "launchpad-probe"
        type: "application"
        targetName: "launchpad-probe-" + simplelink.board.type + ".elf"

        Depends { name: "simplelink"; submodules: [ "core", "drivers", "tirtos", "board" ] }
        Depends { name: "shared-types" }
        Depends { name: "qstbuildconfig" }

        simplelink.core.device: "cc1310f128"
        simplelink.board.type: "CC1310_LAUNCHXL"
        simplelink.board.useDefaultConfig: false

        cpp.cxxLanguageVersion: "c++14"

        Properties {
           condition: qbs.buildVariant === "debug"
           cpp.commonCompilerFlags: "-g"
        }

        qbs.optimization: "none"

        files: [
            "arraylist.h",
            "cc13xx-cc26xx-driver-config.c",
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

