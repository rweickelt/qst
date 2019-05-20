import qbs

Product {

    condition : qbs.architecture.startsWith("x86")

    files: [
        "imports/QstAutoTest.qbs",
        "modules/simplelink/SimpleLinkModule.qbs",
        "modules/simplelink/device.js",
        "modules/simplelink/board/board.qbs",
        "modules/simplelink/core/core.qbs",
        "modules/simplelink/drivers/drivers.qbs",
        "modules/simplelink/tirtos/tirtos.qbs"
    ]
	name : "qbs-items"
	
}
