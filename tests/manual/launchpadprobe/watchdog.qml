import qst 1.0
import ti 1.0

LaunchpadProbeTest {
    name: "watchdog"

    property int connectionTimeoutMs: 600
    property int pingIntervalMs: 517
    property int timeUntilDutKillMs: Math.ceil(Math.random() * connectionTimeoutMs) + 100

    // Shows the DUT's connected state
    PinProbe {
        name: "connected-led"
        id: connectedLed
        ioid: 22
        type: PinProbe.Read
        value: PinProbe.Low
        pullMode: PinProbe.PullDown
        port:  Xds.portFromSerial(profile.probe.serial)
    }

    // Shows the DUT's activity
    PinProbe {
        name: "activity-led"
        id: activityLed
        ioid: 21
        type: PinProbe.Read
        value: PinProbe.Low
        pullMode: PinProbe.PullDown
        port:  Xds.portFromSerial(profile.probe.serial)
    }

    // Another qst instance accessing the DUT board
    ProcessProbe {
        id: dutQstProcess
        name: "dut-qst-process"
        program: profile.qstExecutable
        arguments: [
            "run",
            "-f",
            path + "/watchdog-dut.qml",
            "-p",
            profile.name
        ]

        onFinished: {
            if (exitCode === 1) {
                console.log(readAllStandardError())
            } else if (exitCode === 2) {
                console.log(readAllStandardOutput())
            }
        }

        // Process termination in run() must provoke a crash.
        // This overrides default error handling of ProcessProbe
        // which would cause an immediate test fail instead.
        onErrorOccured: Qst.compare(error, ProcessProbe.Crashed)
    }

    SignalProbe {
        id: dutConnected
        signal: connectedLed.valueChanged
        condition: connectedLed.value === PinProbe.High

        onCountChanged: Qst.compare(count, 1)
    }

    SignalProbe {
        id: dutDisconnected
        signal: connectedLed.valueChanged
        condition: connectedLed.value === PinProbe.Low

        onCountChanged: Qst.compare(count, 1)
    }

    signal dutKilled()

    DurationConstraint {
        id: connectionTimeoutConstraint
        name: "activity-led-duration-constraint"
        beginOn: dutKilled
        endOn:   dutDisconnected

        minDuration: (connectionTimeoutMs - pingIntervalMs)
        maxDuration: (connectionTimeoutMs + pingIntervalMs)
    }

    function run() {
        // The DUT must not be in connected state
        Qst.compare(connectedLed.value, PinProbe.Low)
        // Start another qst application that connects to the DUT
        dutQstProcess.start()
        Qst.wait(300)
        // The DUT must now be in connected state
        Qst.compare(connectedLed.value, PinProbe.High)

        // Kill the Qst DUT instance after some time to
        // trigger a watchdog reset
        Qst.wait(timeUntilDutKillMs)
        dutQstProcess.terminate()

        // Wait for the termination propagate
        dutQstProcess.waitForFinished(300)
        // Qst.compare(dutQstProcess.state, ProcessProbe.NotRunning)

        // Wait and ensure that the DUT resets
        Qst.wait(connectionTimeoutMs + (2*pingIntervalMs))
        Qst.compare(connectedLed.value, PinProbe.Low)
    }
}
