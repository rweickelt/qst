import qst 1.0
import ti 1.0

Testcase {
    name: "watchdog"

    property int connectionTimeoutMs: 600
    property int pingIntervalMs: 517
    property int timeUntilDutKillMs: Math.ceil(Math.random() * connectionTimeoutMs) + 100

    PinProbe {
        name: "connected-led"
        id: connectedLed
        ioid: 22
        type: PinProbe.Read
        value: PinProbe.Low
        pullMode: PinProbe.PullDown
        port:  Xds.portFromSerial(profile.probe.serial)
    }

    PinProbe {
        name: "activity-led"
        id: activityLed
        ioid: 21
        type: PinProbe.Read
        value: PinProbe.Low
        pullMode: PinProbe.PullDown
        port:  Xds.portFromSerial(profile.probe.serial)
    }

    ProcessProbe {
        id: dutQstProcess
        name: "dut-qst-process"
        program: project.qstExecutable
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
        maxDuration: connectionTimeoutMs
    }

    function run() {
        Qst.compare(connectedLed.value, PinProbe.Low)
        dutQstProcess.start()
        Qst.wait(100)
        Qst.compare(connectedLed.value, PinProbe.High)
        Qst.wait(timeUntilDutKillMs)
        dutQstProcess.terminate()
        dutKilled()
        dutQstProcess.waitForFinished(100)
        Qst.compare(dutQstProcess.state, ProcessProbe.NotRunning)
        Qst.wait(connectionTimeoutMs * 2)
        Qst.compare(connectedLed.value, PinProbe.Low)
    }
}
