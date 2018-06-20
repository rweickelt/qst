import qst 1.0
import ti 1.0

Testcase {
    property int repetitions: 500

    name: "benchmark-response-time"

    PinProbe {
        id: reader
        name: "reader"
        ioid: profile.probe.ioid
        type: PinProbe.Read
        value: PinProbe.Undefined
        pullMode: PinProbe.PullDisabled
        port: Xds.portFromSerial(profile.probe.serial)
    }

    PinProbe {
        id: writer
        name: "writer"
        ioid: profile.dut.ioid
        type: PinProbe.Write
        value: PinProbe.Low
        pullMode: PinProbe.PullDisabled
        port:  Xds.portFromSerial(profile.dut.serial)
    }

    onFinished: Qst.wait(200)

    function run() {
        var responseTimes = []
        var newValue = PinProbe.High

        for (var i = 0; i < repetitions; i++) {
            var begin = test.elapsedTime
            writer.value = newValue
            while (reader.value != newValue) {
                Qst.wait(1)
            }
            var end = test.elapsedTime
            var responseTime = end - begin
            responseTimes.push(responseTime)

            newValue = (newValue === PinProbe.High) ? PinProbe.Low : PinProbe.High
        }

        responseTimes.sort(function(a, b){ return a - b })
        var minResponseTime = responseTimes[0]
        var maxResponseTime = responseTimes[responseTimes.length - 1]
        var avgResponseTime = arrayAvg(responseTimes)

        console.log("Avg " + avgResponseTime + " min " + minResponseTime + " max " + maxResponseTime)

        var counts = count(responseTimes)
        counts.forEach(function(element){
            console.log("value " + element.value + " count " + element.count)
        });
    }

    function count(values) {
        if (values.length === 0) {
            return {}
        }

        var init = [{"value": values[0], "count": 0}]
        var counts = values.reduce(function(acc, value) {
            var last = acc.length - 1
            if (acc[last].value !== value) {
                acc.push({"value": value, "count": 1})
            } else {
                acc[last].count++
            }
            return acc
        }, init)

        return counts
    }

    function arrayAvg(array) {
        var sum = array.reduce(function(acc, value) { return acc + value; })
        return sum / array.length
    }

    function arrayMin(array) {
      return array.reduce(function (left, right) {
        return ( left < right ? left : right )
      })
    }

    function arrayMax(array) {
      return array.reduce(function (left, right) {
        return ( left > right ? left : right )
      })
    }
}
