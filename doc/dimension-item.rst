Dimension Item
==============

..  cpp:class:: Dimension

    Defines a value span within a :class:`Matrix`.

..  cpp:namespace:: Dimension

========================== ==========================
Possible locations         :cpp:class:`Matrix`
========================== ==========================


Detailed Description
--------------------

..  cpp:namespace:: Dimension

The ``Dimension`` item defines a span of one or more property values. It can
only exist in a :cpp:class:`Matrix` context. The size of a ``Dimension`` is
determined by its values. The item does not have any predefined properties and
it is not intended to `declare` new properties using the ``property`` keyword.
Instead, it is enough to write a property `name` and assign `values` to it.

The property `name` must be present in all test cases that the
:cpp:class:`Matrix` is applied to. If the test case does not have a property
with that name, an error is thrown.

The `values` must be supplied either in an array with a defined length or as a
trivial type.

Single array
~~~~~~~~~~~~

A typical ``Dimension`` component defines a single property in array format::

    Matrix {
        testcases: "*"

        Dimension {
            mcu: [
                "cc1312R1",
                "cc1352R1",
                "cc1352P1",
            ]
        }
    }

Multiple arrays
~~~~~~~~~~~~~~~

It is also possible to define multiple arrays in one dimension as long as they
have equal length. The values are read as tuples, for instance::

    Dimension {
        mcu: [
            "cc1312R1",
            "cc1352R1",
            "cc1352P1",
        ]
        board: [
            "CC1312R1_LAUNCHXL",
            "CC1352R1_LAUNCHXL",
            "CC1352P1_LAUNCHXL"
        ]
    }

results in the tag pairs:

- :code:`Testcase { mcu: "cc1312R1"; board: "CC1312R1_LAUNCHXL"`
- :code:`Testcase { mcu: "cc1352R1"; board: "CC1352R1_LAUNCHXL"`
- :code:`Testcase { mcu: "cc1352P1"; board: "CC1352P1_LAUNCHXL"`


Arrays and trivial values
~~~~~~~~~~~~~~~~~~~~~~~~~

For convenience it is possible to shrink arrays with identical values as long as
the ``Dimension`` contains an array that determines its length::

    Dimension {
        mcu: [
            "cc1312R1",
            "cc1352R1",
            "cc1352P1",
        ]
        family: "cc13x2" // cloned 3 times
    }

The above ``Dimension`` results in:

- :code:`Testcase { mcu: "cc1312R1"; family: "cc13x2" }`
- :code:`Testcase { mcu: "cc1352R1"; family: "cc13x2" }`
- :code:`Testcase { mcu: "cc1352P1"; family: "cc13x2" }`


References and complex expressions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

References to other properties are allowed as well as referencing the whole
array:

..  literalinclude:: code/reference/dimension-with-references.qml
    :name: dimension-with-references.qml
    :start-after: @snip
    :end-before: @snap
    :dedent: 4


It is currently only possible to evaluate JavaScript expressions by referencing
other properties. The following example does not work::

    // Not supported
    Dimension {
        animals: {
            var values = []
            values.push("cat")
            values.push("dog")
            return values
        }
    }