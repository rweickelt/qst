import qbs
import "../SimpleLinkModule.qbs" as SimpleLinkModule

SimpleLinkModule {
    name : "drivers"

    Depends { name : "cpp" }

    cpp.includePaths : [
        simplelink.core.installPath + "/source"
    ]

    cpp.libraryPaths : [
        simplelink.core.installPath + "/source/ti/drivers/lib",
        simplelink.core.installPath + "/source/ti/drivers/rf/lib",
        simplelink.core.installPath + "/source/ti/display/lib",
        simplelink.core.installPath + "/source/ti/mw/lcd/lib"
    ]

    cpp.staticLibraries: [
        ":rf_multiMode_" + simplelink.core.deviceFamily + ".am3g",
        ":lcd.am3g",
        ":display.am3g",
        ":drivers_" + simplelink.core.deviceFamily + ".am3g",
        ":driverlib.lib"
    ]
}
