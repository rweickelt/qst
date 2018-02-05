import qbs 1.0
import "../device.js" as Device

Module {
    property string installPath
    property string deviceFamily : Device.deviceFamilyDir(product.simplelink.core.device)
    property string device


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

    cpp.machineType: Device.machineType(deviceFamily)

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
        installPath + "/source/ti/devices/" + Device.deviceFamilyDir(deviceFamily) + "/driverlib/bin/gcc",
    ]

    cpp.driverFlags: [
        "-mtune=" + Device.mcuType(deviceFamily),
        "-mthumb",
        "--specs=nano.specs",
        "-nostartfiles",
        "-static",
    ]

    cpp.linkerFlags: [
        "--gc-sections",
    ]

    Group {
        name : "startup-files"
        prefix: simplelink.core.installPath + "/source/ti/devices/" + Device.deviceFamilyDir(simplelink.core.deviceFamily) + "/startup_files/"

        files : [
            "ccfg.c"
        ]
    }

    validate: {
        if (Device.deviceFamily(simplelink.core.device) === "unknown")
        {
            console.error("simplelink.core.device " + simplelink.core.device + " is not supported.");
        }
    }

}
