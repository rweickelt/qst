Xds Service
===========

..  cpp:class:: Xds

    Helper for XDS-based debugging interfaces.

..  cpp:namespace:: Xds

:Methods:
    :cpp:func:`availableSerials()`, :cpp:func:`portFromSerial()`


Detailed Description
--------------------

..  cpp:namespace:: Xds


The ``Xds`` service provides convenience functions for XDS-based debugging
interfaces. It can be used, for instance, to identify connected debuggers.

The following example assumes that there is only a single XDS debugger connected
and deduces it's serial port identifier.

::

    import qst 1.0
    import ti 1.0

    Project {

        property string launchpadPort: {
            var serials = Xds.availableSerials();
            Qst.compare(serials.length, 1,
                "Either no or too many probe boards connected");
            return Xds.portFromSerial(serials[0]);
        }

    }

``Xds`` is a singleton type and cannot be instantiated as a QML component.


Methods
-------

..  cpp:function:: stringlist availableSerials()

    Returns the serial numbers of all connected XDS debuggers as an array of
    strings.


..  cpp:function:: string portFromSerial(string serial)

    Returns the port to which a given `serial` is connected to.