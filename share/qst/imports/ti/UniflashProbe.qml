import qst 1.0

Component {
    id : root

    property string installPath : "/opt/ti/uniflash_4.2"
    property string programmer
    property string serial
    property string device
    property string file

    signal finished

    function flash() {
        Qst.verify(File.exists(uniflash.program), uniflash.program + " not found. Check 'installPath' property.");
        Qst.verify(File.exists(file), file + " not found. Check 'file' property.");
        Qst.verify(File.exists((installPath
                + "/deskdb/content/TICloudAgent/linux/ccs_base/common/targetdb/devices/"
                + device + ".xml")), "No db entry for device '"+ device + "' found in '"
                + installPath + "/deskdb/content/TICloudAgent/linux/ccs_base/common/targetdb/devices/" + "'.");

        var configFilePath = test.workingDirectory + "/uniflashprobe"
                + "-" + device
                + "-" + serial
                + ".xml";
        var configFile = new TextFile(configFilePath, TextFile.ReadWrite);
        configFile.truncate();
        configFile.write(xmlconfig);
        configFile.close();

        uniflash.arguments = [
            "--flash",
            "--config=" + configFilePath,
            // "--post-flash-device-cmd", "PinReset", // TCK is toggled anyway, use xds110reset instead
                                                      // see https://e2e.ti.com/support/development_tools/code_composer_studio/f/81/t/665179
            root.file
        ];
        uniflash.start();
    }

    function waitForFinished(milliseconds) {
        var t0 = test.elapsedTime;
        if (!uniflash.waitForFinished(milliseconds)) {
            return false;
        }
        var t1 = test.elapsedTime;
        return reset.waitForFinished(Math.max(milliseconds - (t1 - t0), -1));
    }

    ProcessProbe {
        id: uniflash

        program: installPath + "/dslite" + ((Qst.hostOS === "windows") ? ".bat" : ".sh")

        onFinished: {
            Qst.compare(exitCode, 0, "UniflashProbe '" + root.name
                    + "' (dslite) exited with " + exitCode
                    + ": " + readAllStandardError() + readAllStandardOutput());
            reset.start();
            reset.waitForStarted(100);
        }
     }

    ProcessProbe {
        id: reset

        program:  installPath + "/simplelink/gen2/bin/xds110reset" + ((Qst.hostOS === "windows") ? ".exe" : "")
        arguments: [
            "-s",
            root.serial
        ]

        onFinished: {
            Qst.compare(exitCode, 0, "UniflashProbe '" + root.name
                    + "' (xds110reset) exited with " + exitCode
                    + ": " + readAllStandardError() + readAllStandardOutput());
            root.finished();
        }
    }

    readonly property string xmlconfig :
        '<?xml version="1.0" encoding="UTF-8" standalone="no"?>
            <configurations XML_version="1.2" id="configurations_0">
                <configuration XML_version="1.2" id="configuration_0">
                <instance XML_version="1.2" desc="Texas Instruments XDS110 USB Debug Probe" href="connections/TIXDS110_Connection.xml" id="Texas Instruments XDS110 USB Debug Probe" xml="TIXDS110_Connection.xml" xmlpath="connections"/>
                <connection XML_version="1.2" id="Texas Instruments XDS110 USB Debug Probe">
                <instance XML_version="1.2" href="drivers/tixds510icepick_c.xml" id="drivers" xml="tixds510icepick_c.xml" xmlpath="drivers"/>
            <instance XML_version="1.2" href="drivers/tixds510cs_dap.xml" id="drivers" xml="tixds510cs_dap.xml" xmlpath="drivers"/>
            <instance XML_version="1.2" href="drivers/tixds510cortexM.xml" id="drivers" xml="tixds510cortexM.xml" xmlpath="drivers"/>
            <property Type="choicelist" Value="4" id="SWD Mode Settings">
                <choice Name="cJTAG (1149.7) 2-pin advanced modes" value="enable">
                    <property Type="choicelist" Value="1" id="XDS110 Aux Port"/>
                </choice>
            </property>
            <property Type="choicelist" Value="1" id="Debug Probe Selection">
                <choice Name="Select by serial number" value="0">
                    <property Type="stringfield" Value="' + root.serial + '" id="-- Enter the serial number"/>
                </choice>
            </property>
            <platform XML_version="1.2" id="platform_0">
                <instance XML_version="1.2" desc="' + root.device.toUpperCase() + '" href="devices/' + root.device.toLowerCase() + '.xml" id="' + root.device.toUpperCase() + '" xml="' + root.device.toLowerCase() + '.xml" xmlpath="devices"/>
            </platform>
        </connection>
    </configuration>
</configurations>'
}
