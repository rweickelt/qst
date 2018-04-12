PinProbe Item
=============

..  cpp:class:: PinProbe

    Represents a digital IO pin on the launchpad probe board.

..  cpp:namespace:: PinProbe

..  rubric:: Enumerations

- :cpp:enum:`PullMode`
- :cpp:enum:`Type`
- :cpp:enum:`Value`

..  rubric:: Properties

- :cpp:member:`ioid`
- :cpp:member:`port`
- :cpp:member:`pullMode`
- :cpp:member:`type`
- :cpp:member:`value`


Detailed Description
--------------------

..  cpp:namespace:: PinProbe

A `PinProbe` component represents an IO pin on the launchpad probe board. It can
be used to physically interface Qst to a Device Under Test (DUT) and
stimulate/analyse digital hardware signals.

Each `PinProbe` item can be configured either as input or output. Additional
pull-up or pull-down resistors may be activated for inputs. The output state is
set by writing to the ``output`` property and events on the probe wire can be
observed by attaching a signal handler to the ``valueChanged()`` signal.

Example::

    import qst 1.0

    // Assuming a DUT with an LED and a button and two wires of a
    // launchpad probe board attached.
    // On a falling edge of the button IO, the LED shall be turned on.
    Testcase {
        name: "test-app"

        property string port: "ttyACM3"

        PinProbe {
            id: button
            port: test.port
            type: PinProbe.Write
            value: PinProbe.High
        }

        PinProbe {
            id: led
            port: test.port
            type: PinProbe.Read

            onValueChanged: {
                Qst.info("The LED value is now " + led.value);
            }
        }

        function run() {
            Qst.wait(50);
            Qst.compare(led.value, PinProbe.Low, "LED should be off but is on");
            // Stimulate button press
            button.value = PinProbe.Low;
            Qst.wait(5);
            Qst.compare(led.value, PinProbe.High, "LED should be on but is off");
        }
    }

`PinProbe` currently only emits the :cpp:func:`valueChanged()` signal. All other
properties are assumed to be constant during application life time.


Enumerations
------------

..  cpp:enum:: Type

    Configures the pin direction.

    ..  cpp:enumerator:: Read

        Configures the pin as input.

    ..  cpp:enumerator:: Write

        Configures the pin as output.


..  cpp:enum:: PullMode

    Whether internal pull resistors should be enabled or not.

    ..  cpp:enumerator:: PullDisabled

        Pull resistors are disabled.

    ..  cpp:enumerator:: PullDown

        Enables a pull-down resistor on the pin.

    ..  cpp:enumerator:: PullUp

        Enables a pull-up resistor on the pin.


..  cpp:enum:: Value

    The logic level on the hardware pin.

    ..  cpp:enumerator:: Low

        Equals to 0 and false.

    ..  cpp:enumerator:: High

        Equals to 1 and true.

    ..  cpp:enumerator:: Undefined

        No value has been assigned to this pin yet.


Properties
----------

..  cpp:member:: int ioid

    :default: 0

    Specifies the IO identifier on the probe board. The range is usually 0..31,
    but not all IOs might be available.


..  cpp:member:: string port

    :default: empty

    The serial port identifier of the probe board. On Windows, this is usually
    ``COMx`` while on Linux systems ``ttyACMx`` or ``ttyUSBx`` is commonly used.

    See also :cpp:func:`Xds::portFromSerial()`


..  cpp:member:: PullMode pullMode

    :default: :cpp:enumerator:`PullMode::PullDisabled`

    Configures the pin access direction. When configured to
    :cpp:enumerator:`Type::Read`, the pin probe will detect negative and
    positive edges on the pin.


..  cpp:member:: Type type

    :default: :cpp:enumerator:`Type::Read`

    Configures the pin access direction. When configured to
    :cpp:enumerator:`Type::Read`, the pin probe will detect negative and
    positive edges on the pin.


..  cpp:member:: Value value

    :default: :cpp:enumerator:`Value::Undefined`

    The current state of the pin. The property can be read from and written to.
    A write has no effect when the pin is configured as
    :cpp:enumerator:`Type::Read`. Although defined as an enumerator, values are
    implicitly converted to integers and booleans.
