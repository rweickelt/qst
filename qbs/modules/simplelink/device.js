function deviceFamilyDir(family) {
    if (family === "cc13x2")
        return "cc13x2_cc26x2_v1";
    else
        return family;
}

function machineType(family) {
    if (deviceFamily === "cc13x0")
        return "armv7-m";
    else if (deviceFamily === "cc13x2")
        return "armv7-m";
}

function mcuType(family) {
    if (deviceFamily === "cc13x0")
        return "cortex-m3";
    else if (deviceFamily === "cc13x2")
        return "cortex-m4";
}
