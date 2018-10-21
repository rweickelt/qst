/*
This test case installs the launchpad probe firmware on a launchpad.

The firmware is bundled with Qst in the subfolder share/qst/firmware/.

1. Make sure that Uniflash is correctly installed.
2. Modify below properties depending on your installation.
3. Connect a launchpad
4. Run this test case: qst run -f install-launchpad-probe.qml

*/

import QtQml 2.2
import qst 1.0
import ti 1.0

Testcase {
    id: test
    name: "install-launchpad-probe"

    property string uniflashInstallPath: "/opt/ti/uniflash_4.2"
    property string firmwarePath: "/opt/qst/share/qst/firmware/launchpad-probe-CC1310_LAUNCHXL.elf"
    property string device: "cc1310f128"

    property var serials: Xds.availableSerials()

    property list<Component> programmers: [
        UniflashProbe {
            installPath: test.uniflashInstallPath
            device: test.device
            file: test.firmwarePath
        },
        UniflashProbe {
            installPath: test.uniflashInstallPath
            device: test.device
            file: test.firmwarePath
        },
        UniflashProbe {
            installPath: test.uniflashInstallPath
            device: test.device
            file: test.firmwarePath
        }
    ]

    function run() {
        Qst.verify(File.exists(uniflashInstallPath), uniflashInstallPath + " not found. Check 'uniflashInstallPath' property.");
        Qst.verify(File.exists(firmwarePath), firmwarePath + " not found. Check 'firmwarePath' property.");

        Qst.verify(serials.length > 0, "No XDS110 detected")
        Qst.info("Detected " + serials)
        Qst.verify(serials.length <= 3, "Too many XDS110 detected")

        for (var i = 0; i < serials.length; i++) {
            programmers[i].serial = serials[i]
            programmers[i].flash()
        }

        programmers[programmers.length - 1].waitForFinished(10000);
    }
}
