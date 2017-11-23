Project Item
============

A container item for multiple :cpp:class:`TestCase` items.

Properties
----------

- :cpp:member:`Project::name`
- :cpp:member:`Project::references`
- :cpp:member:`Project::workingDirectory`


Signals
-------

Methods
-------


Detailed Description
--------------------

..  cpp:class:: Project

    A ``Project`` item represents a collection of of test cases.
    In a non-trivial project, these test cases are typically defined in their
    own files and referenced in the main project file::

        Project {
            references: [
                "testcase1.qml",
                "testcase2.qml"
            ]
        }

    Any property ``prop`` attached to this item is available in sub-items as
    ``project.prop``.


..  cpp:member:: string Project::name

    :default: empty string

    Name of the project. This value does currently have no effect.


..  cpp:member:: stringlist Project::references

    :default: empty list

    A list of files containing test cases. Test cases are executed in the
    specified order.  Paths are relative to the project file unless an
    absolute path is provided.::

      references : [
          "testcase-1.qml",         // Path relative to the project file
          "/path/to/testcase-2.qml" // Absolute file path
      ]


..  cpp:member:: const string Project::workingDirectory

    :default: ``project.workingDirectory + "/" + name``

    Project-wide directory where all test cases are physically executed.
    This property is specified as a command line argument
    ``--working-directory``.


