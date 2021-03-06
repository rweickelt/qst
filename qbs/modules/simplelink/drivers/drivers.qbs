import qbs
import "../SimpleLinkModule.qbs" as SimpleLinkModule

SimpleLinkModule {
    name : "drivers"
    property string installPath : product.simplelink.core.installPath
    property bool usePrebuilt: true

    Depends { name : "cpp" }
    Depends { name: "simplelink.core" }

    cpp.libraryPaths : [
        product.simplelink.drivers.installPath + "/source/ti/drivers/lib",
        product.simplelink.drivers.installPath + "/source/ti/drivers/rf/lib",
        product.simplelink.drivers.installPath + "/source/ti/display/lib",
        product.simplelink.drivers.installPath + "/source/ti/mw/lcd/lib"
    ]

    Properties {
        condition: product.simplelink.drivers.usePrebuilt === true

        cpp.staticLibraries: [
            ":rf_multiMode_" + simplelink.core.deviceFamily + ".am3g",
            ":lcd.am3g",
            ":display.am3g",
            ":drivers_" + simplelink.core.deviceFamily + ".am3g",
            ":driverlib.lib"
        ]

    }
}
