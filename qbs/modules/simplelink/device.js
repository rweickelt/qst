
var devices = [
    { "device": "cc1310f32", "family": "cc13x0" },
    { "device": "cc1310f64", "family": "cc13x0" },
    { "device": "cc1310f128", "family": "cc13x0" },
    { "device": "cc1350f128", "family": "cc13x0" },
    { "device": "cc1352r1f3", "family": "cc13x2" },
]

function deviceFamily(device) {
    for (var key in devices) {
        if (devices[key].device === device) {
            return devices[key].family;
        }
    }
    console.error("Device " + device + " unknown or unsupported");
    return "unknown";
}

function deviceFamilyDir(family) {
    if (family === "cc13x2")
        return "cc13x2_cc26x2_v1";
    else
        return family;
}

function machineType(family) {
    if (family === "cc13x0")
        return "armv7-m";
    else if (family === "cc13x2")
        return "armv7-m";
}

function mcuType(family) {
    if (family === "cc13x0")
        return "cortex-m3";
    else if (family === "cc13x2")
        return "cortex-m4";
}
