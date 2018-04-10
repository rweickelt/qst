Hardware probes
===============

Qst can be connected to hardware in order to perform measurements or to control
the device under test.


Texas Instruments Launchpad
---------------------------

The launchpad probe firmware converts a launchpad into a cheap real-time IO
interface. Available are all pins on the board except the LEDs at DIO6 and DIO7
which are occupied by the probe firmware.

Supported launchpads:

- `CC1310_LAUNCHXL <http://www.ti.com/tool/LAUNCHXL-CC1310>`_
- `CC1350_LAUNCHXL <http://www.ti.com/tool/LAUNCHXL-CC1350>`_

Related probe items:

- :cpp:class:`PinProbe`
- :cpp:class:`UniflashProbe`

Related service items:

- :cpp:class:`Xds`