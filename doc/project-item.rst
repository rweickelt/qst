Project Item
============

..  cpp:class:: Project

    A container item for multiple :cpp:class:`Testcase` items.

..  cpp:namespace:: Testcase

..  rubric:: Properties

- :cpp:member:`name`
- :cpp:member:`references`
- :cpp:member:`workingDirectory`


Detailed Description
--------------------

..  cpp:namespace:: Testcase

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


Properties
----------

..  cpp:member:: string name

    :default: empty string

    Name of the project. This value does currently have no effect.


..  cpp:member:: stringlist references

    :default: empty list

    A list of files containing test cases. Test cases are executed in the
    specified order.  Paths are relative to the project file unless an
    absolute path is provided::

      references : [
          "testcase-1.qml",         // Path relative to the project file
          "/path/to/testcase-2.qml" // Absolute file path
      ]


..  cpp:member:: const string workingDirectory

    Project-wide directory where all test cases are physically executed.

    The default value is defined by ``--working-directory`` and cannot be
    changed from within QML.


