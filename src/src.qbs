import qbs

Project {
    name : "sources"

    references: [
        "qst/qst.qbs",
        "launchpad-probe/launchpad-probe.qbs",
        "shared-types/shared-types.qbs"
    ]
}
