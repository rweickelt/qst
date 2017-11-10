import qbs
import "../SimpleLinkModule.qbs" as SimpleLinkModule

SimpleLinkModule {
    name : "board"
    property string type

    Depends { name : "cpp" }

    cpp.includePaths : [
        simplelink.core.installPath + "/source/ti/boards/" + type + "/"
    ]

    Group {
        name : "CC1310_LAUNCHXL"
        prefix: simplelink.core.installPath + "/source/ti/boards/CC1310_LAUNCHXL/"
        files : [
            "CC1310_LAUNCHXL.h",
            "CC1310_LAUNCHXL.c",
            "Board.h",
        ]
     }

    Group {
        name : "linker-scripts"
        prefix : simplelink.core.installPath + "/source/ti/boards/CC1310_LAUNCHXL/"
        files : [
            "CC1310_LAUNCHXL_TIRTOS.lds"
        ]
        fileTags: "board_linkerscript"
    }
}
