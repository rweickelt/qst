import qst 1.0
import ti 1.0

/*
Test setup:
 - 2 pins on 1 or 2 different probe boards connected to each other.
 - One is called writer, the other one reader.

Test description:
 - Configures reader and writer in all kinds of combinations involving all features
 - Stimulates signal changes on the writer and checks that they are correctly populated to the reader
 - Test might give strange and misleading results when the probe firmware is completely broken.

*/
Testcase {
    name : "readwrite"
    property int maxResponseTimeMs: 120 /* Maximum time from changing the value on the writer probe until the
                                          valueChanged signal is populated on the reader probe.
                                          The duration is huge. The average value is 4 ms, but I observed
                                          very long delays up to 120ms sometimes.
                                          Seems to be caused by the Linux scheduler. */

    PinProbe {
        id: reader
        name: "reader"
        ioid: 23
        type: PinProbe.Read
        value: PinProbe.Undefined
        pullMode: PinProbe.PullDisabled
        port: Xds.portFromSerial(project.probeSerial)
    }

    PinProbe {
        id: writer
        name: "writer"
        ioid: 13
        type: PinProbe.Write
        value: PinProbe.Low
        pullMode: PinProbe.PullDisabled
        port: Xds.portFromSerial(project.dutSerial)
    }

    SignalProbe { id: readerTypeChanged;     signal: reader.typeChanged }
    SignalProbe { id: readerPullModeChanged; signal: reader.pullModeChanged }
    SignalProbe { id: readerValueChanged;    signal: reader.valueChanged }

    SignalProbe { id: writerTypeChanged;     signal: writer.typeChanged }
    SignalProbe { id: writerPullModeChanged; signal: writer.pullModeChanged }
    SignalProbe { id: writerValueChanged;    signal: writer.valueChanged }

    // We want to know when the estimated value maxResponseTimeMs is not good enough.
    DurationConstraint {
        id: responseTimeConstraint
        name: "responeTimeConstraint"
        from: SignalProbe { signal: writer.valueChanged }
        to:   SignalProbe { signal: reader.valueChanged }
        enabled: false
        minDuration: 0
        maxDuration: maxResponseTimeMs
    }

    function clearSignalProbes() {
        readerPullModeChanged.clear()
        readerTypeChanged.clear()
        readerValueChanged.clear()
        writerPullModeChanged.clear()
        writerTypeChanged.clear()
        writerValueChanged.clear()
    }

    function run() {
        // Prepare writer
        writer.type = PinProbe.Write
        writer.value = PinProbe.Low
        Qst.wait(maxResponseTimeMs)
        Qst.compare(writerPullModeChanged.count, 0)
        Qst.compare(writerTypeChanged.count, 0)
        // Pins were floating until now. The valueChanged signals wouldn't give us any information.
        readerValueChanged.clear()
        writerValueChanged.clear()
        writer.value = PinProbe.Low
        writer.pullMode = PinProbe.PullDisabled
        Qst.wait(maxResponseTimeMs)
        Qst.compare(readerPullModeChanged.count, 0)
        Qst.compare(readerTypeChanged.count, 0)
        Qst.compare(readerValueChanged.count, 0)

        // Prepare reader
        reader.type = PinProbe.Read
        Qst.wait(maxResponseTimeMs)
        Qst.compare(reader.type, PinProbe.Read)
        Qst.compare(readerTypeChanged.count, 0)
        Qst.compare(readerPullModeChanged.count, 0)
        Qst.compare(readerValueChanged.count, 0)
        reader.pullMode = PinProbe.PullDisabled
        Qst.wait(maxResponseTimeMs)
        Qst.compare(reader.pullMode, PinProbe.PullDisabled)
        Qst.compare(readerPullModeChanged.count, 0)
        Qst.compare(readerTypeChanged.count, 0)
        Qst.compare(readerValueChanged.count, 0)
        Qst.compare(reader.value, PinProbe.Low)

        responseTimeConstraint.enabled = true

        // Test read / positive edges
        clearSignalProbes()
        writer.value = PinProbe.High
        Qst.wait(maxResponseTimeMs)
        Qst.compare(reader.value, PinProbe.High)
        Qst.compare(readerValueChanged.count, 1)

        // Test read / negative edges
        clearSignalProbes()
        writer.value = PinProbe.Low
        Qst.wait(maxResponseTimeMs)
        Qst.compare(reader.value, PinProbe.Low)
        Qst.compare(readerValueChanged.count, 1)

        // Test read / PullUp
        clearSignalProbes()
        reader.pullMode = PinProbe.PullUp
        Qst.wait(maxResponseTimeMs)
        Qst.compare(reader.pullMode, PinProbe.PullUp)
        Qst.compare(readerPullModeChanged.count, 1)
        Qst.compare(reader.value, PinProbe.Low)
        // Should be still pulled low by writer
        Qst.compare(readerValueChanged.count, 0)
        writer.pullMode = PinProbe.PullDisabled
        writer.type = PinProbe.Read
        Qst.compare(writerTypeChanged.count, 1)
        // Stimulus pin should be free floating now, pulled by reader.
        Qst.wait(maxResponseTimeMs)
        Qst.compare(reader.value, PinProbe.High)
        Qst.compare(readerValueChanged.count, 1)

        // Test read / PullDown
        clearSignalProbes()
        reader.pullMode = PinProbe.PullDown
        Qst.wait(maxResponseTimeMs)
        Qst.compare(reader.pullMode, PinProbe.PullDown)
        Qst.compare(readerPullModeChanged.count, 1)
        Qst.compare(reader.value, PinProbe.Low)
        Qst.compare(readerValueChanged.count, 1)
        clearSignalProbes()
    }
}
