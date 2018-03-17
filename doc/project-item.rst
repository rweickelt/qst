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

..  cpp:namespace:: Project

A ``Project`` item represents a collection of of test cases. Test cases can be
defined in two ways:

 1. **in-line**

    Usually used for a bunch of trivial test cases with only few lines or
    if complex test cases are instantiated multiple times with slightly
    different configuration.

    The test cases are executed in undefined order.

    Example in-line project::

        Project {
            name: "inline-project"

            Testcase {
                name: "trivial-1"
                function run() {
                    // Qst.verify(...)
                }
            }

            // Assuming that ComplexTestcase.qml exists in the current
            // directory
            ComplexTestcase { name: "complex-1"; config: "something" }
            ComplexTestcase { name: "complex-2"; config: "something-else" }

        }

 2. **as references**

    Usually used in non-trivial projects. Test cases are defined in their own
    files and then referenced by the main project file. Any property ``prop``
    attached to the project item is available in sub-items as ``project.prop``.

    The execution order is defined by their occurrence order in the
    :cpp:member:`references` item.

    Example references project::

        Project {
            name: "referencing-project"
            property string someProperty: "sometext"

            // someProperty is available in all test cases as
            // project.someProperty
            references: [
                "testcase1.qml",
                "testcase2.qml"
            ]
        }


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


