import qbs

Module {
    name: "qbsbuildconfig"
    property bool runAutotest: false
    property bool installTests: false

    qbs.installPrefix: ""
}
