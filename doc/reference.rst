Reference
=========

..  rubric:: General

.. toctree::
   :maxdepth: 2

   command-line-interface
   language


.. toctree::
   :maxdepth: 1
   :hidden:
   :glob:

   *-item
   *-service

..  rubric:: Language Items

======================================================== ===========
Name                                                     Description
======================================================== ===========
:doc:`Dimension <dimension-item>`
:doc:`Matrix <matrix-item>`
:doc:`Project <project-item>`                            A container item for multiple :cpp:class:`Testcase` items.
:doc:`Testcase <testcase-item>`                          Implements a test action.
======================================================== ===========


..  rubric:: Convenience Items

======================================================== ===========
Name                                                     Description
======================================================== ===========
:doc:`DurationConstraint <durationconstraint-item>`      Checks the time between two signal occurrences.
:doc:`PinProbe <pinprobe-item>`                          Represents a digital IO pin on the launchpad probe board.
:doc:`SignalProbe <signalprobe-item>`                    Watches signals from other items.
:doc:`ProcessProbe <processprobe-item>`                  Starts and watches processes.
:doc:`UniflashProbe <uniflashprobe-item>`                Flash programmer for Texas Instruments MCUs.
:doc:`ValueRangeConstraint <valuerangeconstraint-item>`  Checks whether a property is in a certain range.
======================================================== ===========


.. _services:
..  rubric:: Services

================================== ===========
Name                               Description
================================== ===========
:doc:`File <file-service>`         Provides access to the file system.
:doc:`Qst <qst-service>`           Helper functions for test execution.
:doc:`TextFile <textfile-service>` Reads from and writes into text files.
:doc:`Xds <xds-service>`           Represents XDS-based debugging interfaces.
================================== ===========
