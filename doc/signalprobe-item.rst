SignalProbe Item
=================

..  cpp:class:: SignalProbe

    Watches signals from other items.

..  cpp:namespace:: SignalProbe

:Inherits:
    :qml:item:`Component`

:Properties:
    :cpp:member:`condition`, :cpp:member:`count`, :cpp:member:`signal`

:Methods:
    :cpp:func:`clear()`, :cpp:func:`connect()`

:Signals:
    :cpp:func:`triggered()`


Detailed Description
--------------------

..  cpp:namespace:: SignalProbe

``SignalProbe`` can be used to watch signals of other items. A typical use-case
is a counter for asynchronous events::

    import qst 1.0
    import QtQml 2.0 as QtQml

    Testcase {
        QtQml.Timer {
            id : timer
            interval: 5
            periodic: true
        }

        SignalProbe {
            id: probe
            signal: timer.triggered
        }

        function run() {
            Qst.compare(probe.count, 0, "Timer triggered too early");
            timer.start();
            Qst.wait(50);
            timer.stop();
            Qst.verify(probe.count >= 10, "Timer triggered only "
                + probe.count + " times");
        }


The following example shows, how ``SignalProbe`` can be used, to attach to a
signal that is not directly accessible:

..  code-block:: qml
    :caption: `BaseTestcase.qml`

    import qst 1.0

    Testcase {
        property var pinProbe : PinProbe {
            // properties
            // ...

            // Can attach directly to the valueChanged() signal.
            onValueChanged: {
                Qst.info("pin value is " + pinProbe.value)
            }
        }
    }

..  code-block:: qml
    :caption: `ExtendedTestcase.qml`
    :name: signalprobe_ExtendedTestcase

    import qst 1.0

    BaseTestcase {
        // Cannot attach directly to pinProbe's valueChanged() signal.
        // Use SignalProbe instead.
        SignalProbe {
            signal: pinProbe.valueChanged
            onTriggered: {
                Qst.info("pin value is " + pinProbe.value)
            }
        }
    }


Properties
----------

..  cpp:member:: bool condition

    :default: true

    A guard for :cpp:member:`signal`. When ``true``, incoming signals increment
    the counter and emit :cpp:func:`triggered()`. When ``false``, the incoming
    signal is ignored.


..  cpp:member:: int count

    Counts how many times the signal has fired with respect to
    :cpp:member:`condition`.


..  cpp:member:: Signal signal

    A signal of another item to attach to during the creation stage.


Methods
-------

..  cpp:function:: void clear()

    Resets :cpp:member:`count` to 0 but leaves the signal connection and the
    condition untouched.


..  cpp:function:: void connect(signalHandler)

    Connects the :cpp:func:`triggered()` signal to a `signalHandler` of another
    item. This makes ``SignalProbe`` look like a QML signal for the other item.

    Example::

        DurationConstraint {
            from: SignalProbe { signal: someItem.someSignal }
            to: someOtherSignal
        }

    In this example, ``DurationConstraint`` tries to connect ``SignalProbe`` to
    itself by calling :cpp:func:`connect()`. It doesn't know that
    ``SignalProbe`` is an item and not a plain QML signal.


Signals
-------

..  cpp:function:: void triggered()

    Emitted when :cpp:member:`signal` is triggered while :cpp:member:`condition`
    is true.
