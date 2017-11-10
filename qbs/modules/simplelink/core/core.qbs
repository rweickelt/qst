import qbs 1.0

Module {
    property string installPath
    property string deviceFamily

    Depends { name : "cpp" }

    cpp.commonCompilerFlags: [
        "-Wno-attributes",
        "-Wno-unused-parameter",
        "-Wno-unused-variable",
        "-ffunction-sections",
        "-fdata-sections",
        "-g",
    ]
    cpp.enableRtti : false
    cpp.positionIndependentCode: false

    cpp.machineType: "armv7-m"

    cpp.defines : [
        "DeviceFamily_" + deviceFamily.toUpperCase()
    ]

    cpp.includePaths : [
        installPath + "/source/",
        cpp.toolchainInstallPath + "/../arm-none-eabi/include/"
    ]

    cpp.staticLibraries: [
        "m",
        "c",
        "c",
        "nosys",
    ]

    cpp.libraryPaths: [
        simplelink.core.installPath + "/source/ti/devices/" + simplelink.core.deviceFamily + "/driverlib/bin/gcc",
    ]

    cpp.driverFlags: [
        "--specs=nano.specs",
        "-nostartfiles",
        "-static",
    ]

    cpp.linkerFlags: [
        "--gc-sections",
    ]

    Group {
        name : "startup-files"
        prefix: product.moduleProperty("simplelink.core", "installPath")
                    + "/source/ti/devices/"
                    + product.moduleProperty("simplelink.core", "deviceFamily") + "/startup_files/"

        files : [
            "ccfg.c"
        ]
    }

}
