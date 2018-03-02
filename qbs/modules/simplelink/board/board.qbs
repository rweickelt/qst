import qbs
import "../SimpleLinkModule.qbs" as SimpleLinkModule

SimpleLinkModule {
    name : "board"
    property string type
    property string installPath : product.simplelink.core.installPath + "/source/ti/boards"
    property bool useDefaultConfig: true

    Depends { name : "cpp" }
    Depends { name : "simplelink.core" }

    cpp.includePaths : [
        product.simplelink.board.installPath + "/../../",
        product.simplelink.board.installPath + "/" + type + "/"
    ]

    Group {
        name : "board-config"
        condition: simplelink.board.useDefaultConfig === true
        prefix: product.simplelink.board.installPath + "/" + product.simplelink.board.type + "/"
        files : [
            "*.h",
            product.simplelink.board.type + ".c",
        ]
     }

    Group {
        name : "board-linker-scripts"
        prefix: product.simplelink.board.installPath + "/" + product.simplelink.board.type + "/"
        files : [
            product.simplelink.board.type + "_TIRTOS.lds"
        ]
        fileTags: "board_linkerscript"
    }
}
