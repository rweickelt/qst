Reference
=========

.. toctree::
   :maxdepth: 1
   :hidden:
   :glob:

   *-*

..  rubric:: Core Items

================================================== ===========
Name                                               Description
================================================== ===========
:doc:`CounterConstraint <counterconstraint-item>`  Checks properties and signals against a threshold.
:doc:`PinProbe <pinprobe-item>`                    Represents a digital IO pin on the launchpad probe board.
:doc:`ProcessProbe <processprobe-item>`            Starts and watches processes.
:doc:`Project <project-item>`                      A container item for multiple :cpp:class:`Testcase` items.
:doc:`SignalProbe <signalprobe-item>`              Watches signals from other items.
:doc:`Testcase <testcase-item>`                    Implements a test action.
================================================== ===========


..  rubric:: Convenience Items

========================================= ===========
Name                                      Description
========================================= ===========
:doc:`UniflashProbe <uniflashprobe-item>` Flash programmer for Texas Instruments MCUs.
========================================= ===========


..  rubric:: Services

================================== ===========
Name                               Description
================================== ===========
:doc:`File <file-service>`         Provides access to the file system.
:doc:`Qst <qst-service>`           Helper functions for test execution.
:doc:`TextFile <textfile-service>` Reads from and writes into text files.
:doc:`Xds <xds-service>`           Represents XDS-based debugging interfaces.
================================== ===========
