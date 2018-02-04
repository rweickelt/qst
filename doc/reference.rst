Reference
=========

Items
-----

.. toctree::
   :maxdepth: 1
   :hidden:
   :glob:

   *-item

Generic Items, available through ``import qst 1.0``

======================================= ===========
Name                                    Description
======================================= ===========
:doc:`ProcessProbe <processprobe-item>` Starts and watches processes.
:doc:`Project <project-item>`           A container item for multiple :cpp:class:`Testcase` items.
:doc:`Testcase <testcase-item>`         Implements a test action.
======================================= ===========


TI-specific Items,  available through ``import ti 1.0``

================================== ===========
Name                               Description
================================== ===========
:doc:`PinProbe <pinprobe-item>`    Represents a digital IO pin on the launchpad probe board.
================================== ===========


Services
--------

.. toctree::
   :maxdepth: 1
   :hidden:
   :glob:

   *-service

Generic Services, available through ``import qst 1.0``

================================== ===========
Name                               Description
---------------------------------- -----------
:doc:`Qst <qst-service>`           Helper functions for test execution.
:doc:`TextFile <textfile-service>` Reads from and writes into text files.
================================== ===========


TI-specific Services, available through ``import ti 1.0``

================================== ===========
Name                               Description
---------------------------------- -----------
:doc:`Xds <xds-service>`           Represents XDS-based debugging interfaces.
================================== ===========


