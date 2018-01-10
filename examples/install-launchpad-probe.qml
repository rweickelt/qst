/*
This test case installs the launchpad probe firmware on a launchpad.

The firmware is bundled with Qst in the subfolder share/qst/firmware/.

1. Make sure that Uniflash is correctly installed.
2. Modify below properties depending on your installation.
3. Connect a launchpad
4. Run this test case: qst run -f install-launchpad-probe.qml

*/


import qst 1.0
import ti 1.0

Testcase {
    id: base
    name: "install-launchpad-probe"

    property string uniflashInstallPath: "/opt/ti/uniflash_4.2"
    property string firmwarePath: "/opt/qst/share/qst/firmware/launchpad-probe-CC1310_LAUNCHXL.elf"
    property string serial: "L40002WG"
    property string device: "cc1310f128"

    UniflashProbe {
        id: uniflash
        installPath: base.uniflashInstallPath
        serial: base.serial
        device: base.device
        file: base.firmwarePath
    }

    function run() {
        Qst.verify(File.exists(uniflashInstallPath), uniflashInstallPath + " not found. Check 'uniflashInstallPath' property.");
        Qst.verify(File.exists(firmwarePath), firmwarePath + " not found. Check 'firmwarePath' property.");

        uniflash.flash();
        uniflash.waitForFinished(10000);
    }
}
