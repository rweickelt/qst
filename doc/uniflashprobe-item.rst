UniflashProbe Item
==================

..  cpp:class:: UniflashProbe : ProcessProbe

    Flash programmer interface for Texas Instruments MCUs.

    Inherits :cpp:class:`ProcessProbe`.

..  cpp:namespace:: UniflashProbe

..  rubric:: Properties

- :cpp:member:`device`
- :cpp:member:`file`
- :cpp:member:`installPath`
- :cpp:member:`programmer`
- :cpp:member:`serial`

..  rubric:: Methods

- :cpp:func:`flash()`


Detailed Description
--------------------

..  cpp:namespace:: UniflashProbe

``UniflashProbe`` provides a convenient interface to `Texas Instruments'
Uniflash <http://www.ti.com/tool/UNIFLASH>`_. It can be used to read from and
write to MCUs via a JTAG programmer.

The current state is experimental. Only XDS110 is supported. Since Uniflash
doesn't have a meaningful command line interface, XML config files are hacked
together.

Example::

    import qst 1.0

    Testcase {

        UniflashProbe {
            id: uniflash

            installPath: "C:\ti\uniflash_4.2"
            serial: Xds.availableSerials()[0]
            device: "cc1352r1f3"
            file: test.workingDirectory + "/myApp.out"
        }

        void run() {
            uniflash.start();
            uniflash.waitForFinished(10000);
        }
    }

See also :cpp:class:`Xds`.


Properties
----------

..  cpp:member:: string device

    The exact MCU type, e.g. cc1310f128, cc1352r1f3. The case doesn't matter.


..  cpp:member:: string file

    Path to a binary file to be read out or to be programmed. The file type has
    to be supported by Uniflash (.out, .bin, .hex).


..  cpp:member:: string installPath

    Installation directory of Uniflash.


..  cpp:member:: string programmer

    :default: XDS110

    The JTAG programmer type.


..  cpp:member:: string serial

    The serial number of the programmer.


Methods
-------

..  cpp:function:: void flash()

    Writes :cpp:member:`file` to a :cpp:member:`device` connected via
    :cpp:member:`programmer`. The method returns immediately and the programming
    process executes in background.

    When done, the :cpp:func:`ProcessProbe::finished()` signal is raised.
