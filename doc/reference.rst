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

..  rubric:: Language items

======================================================== ===========
Name                                                     Description
======================================================== ===========
:doc:`Component <component-item>`                        Base type for various dynamic items.
:doc:`Depends <depends-item>`                            Describes dependencies between :class:`Testcase` items.
:doc:`Dimension <dimension-item>`                        Defines a value span within a :class:`Matrix`.
:doc:`Exports <exports-item>`                            Forwards data to dependent :class:`Testcase` items.
:doc:`Matrix <matrix-item>`                              Replicates :class:`Testcase` components with different property values.
:doc:`Project <project-item>`                            Container item for multiple :cpp:class:`Testcase` items.
:doc:`Testcase <testcase-item>`                          Implements a test action.
======================================================== ===========


..  rubric:: Convenience items

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
