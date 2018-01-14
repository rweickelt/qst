import qst 1.0

ProcessProbe {
    id : probe

    property string installPath : "/opt/ti/uniflash_4.2"
    property string programmer
    property string serial
    property string device
    property string file

    program : installPath + "/dslite" + ((Qst.hostOS === "windows") ? ".bat" : ".sh")

    function flash() {
        Qst.verify(File.exists(program), program + " not found. Check 'installPath' property.");
        Qst.verify(File.exists(file), file + " not found. Check 'file' property.");

        var configFilePath = test.workingDirectory + "/UniFlashProbe"
                + "-" + device
                + "-" + serial
                + ".xml";
        var configFile = new TextFile(configFilePath, TextFile.ReadWrite);
        configFile.truncate();
        configFile.write(xmlconfig);
        configFile.close();

        probe.arguments = [
            "--mode",
            "flash",
            "--config=" + configFilePath,
            probe.file
        ];
        probe.start();
    }

    onFinished: {
        Qst.compare(probe.exitCode, 0, "UniflashProbe '" + probe.name
                + "' exited with " + probe.exitCode
                + ": " + probe.readAllStandardError() + probe.readAllStandardOutput());
    }

    property string xmlconfig :
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>
            <configurations XML_version=\"1.2\" id=\"configurations_0\">
                <configuration XML_version=\"1.2\" id=\"configuration_0\">
                <instance XML_version=\"1.2\" desc=\"Texas Instruments XDS110 USB Debug Probe\" href=\"connections/TIXDS110_Connection.xml\" id=\"Texas Instruments XDS110 USB Debug Probe\" xml=\"TIXDS110_Connection.xml\" xmlpath=\"connections\"/>
                <connection XML_version=\"1.2\" id=\"Texas Instruments XDS110 USB Debug Probe\">
                <instance XML_version=\"1.2\" href=\"drivers/tixds510icepick_c.xml\" id=\"drivers\" xml=\"tixds510icepick_c.xml\" xmlpath=\"drivers\"/>
            <instance XML_version=\"1.2\" href=\"drivers/tixds510cs_dap.xml\" id=\"drivers\" xml=\"tixds510cs_dap.xml\" xmlpath=\"drivers\"/>
            <instance XML_version=\"1.2\" href=\"drivers/tixds510cortexM.xml\" id=\"drivers\" xml=\"tixds510cortexM.xml\" xmlpath=\"drivers\"/>
            <property Type=\"choicelist\" Value=\"4\" id=\"SWD Mode Settings\">
                <choice Name=\"cJTAG (1149.7) 2-pin advanced modes\" value=\"enable\">
                    <property Type=\"choicelist\" Value=\"1\" id=\"XDS110 Aux Port\"/>
                </choice>
            </property>
            <property Type=\"choicelist\" Value=\"1\" id=\"Debug Probe Selection\">
                <choice Name=\"Select by serial number\" value=\"0\">
                    <property Type=\"stringfield\" Value=\"" + probe.serial + "\" id=\"-- Enter the serial number\"/>
                </choice>
            </property>
            <platform XML_version=\"1.2\" id=\"platform_0\">
                <instance XML_version=\"1.2\" desc=\"" + probe.device.toUpperCase() + "\" href=\"devices/" + probe.device.toLowerCase() + ".xml\" id=\"" + probe.device.toUpperCase() + "\" xml=\"" + probe.device.toLowerCase() + ".xml\" xmlpath=\"devices\"/>
            </platform>
        </connection>
    </configuration>
</configurations>"


}
