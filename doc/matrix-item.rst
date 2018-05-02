.. |Dimension| replace:: :cpp:class:`Dimension`
.. |Matrix| replace:: ``Matrix``


Matrix Item
===========

..  cpp:class:: Matrix

    Replicates :class:`Testcase` components with different property values.

..  cpp:namespace:: Matrix

:Locations:
    :cpp:class:`Project`, :cpp:class:`Testcase`, document root

:Nested items:
    :cpp:class:`Dimension`

:Properties:
    :cpp:member:`testcases`


Detailed Description
--------------------

..  cpp:namespace:: Matrix

The |Matrix| item allows to implement data-driven tests and to separate
implementation from input data. It spans a space of values based on one or more
|Dimension| items. The resulting value combinations are called tags. Each tag
may be applied to one or more test cases depending on the
:cpp:member:`testcases` property.

..  literalinclude:: code/reference/matrix-project.qml
    :caption: matrix-project.qml
    :name: matrix-project.qml

The |Matrix| component in :ref:`matrix-project.qml` has two dimensions, each
containing an array of length 2. Thus, the matrix expands to 4 different tag
combinations:

========= =========
dog bites dog moans
cat bites cat moans
========= =========


Matrix execution
~~~~~~~~~~~~~~~~

A tagged test case is re-executed for each tag and every execution entity is
called a job. The job order is generally undefined. Tagging has consequences on
the test case's execution:

1. The test case name in the command line output is followed by the tag data and
   a hash of the tag data.

2. The test case's working directory name is expanded with the hash.

Above :ref:`matrix-project.qml` example results in the following output on
command line:

..  code-block:: console

    $ qst run -f matrix-project.qml
    PASS, normal-test,,,
    INFO, tagged-test 1ms2r6i [ cat moans ], , /matrix-project.qml:27, The cat moans.
    PASS, tagged-test 1ms2r6i [ cat moans ],,,
    INFO, tagged-test 17tca19 [ dog bites ], , /matrix-project.qml:27, The dog bites.
    PASS, tagged-test 17tca19 [ dog bites ],,,
    INFO, tagged-test 0ni1i5d [ cat bites ], , /matrix-project.qml:27, The cat bites.
    PASS, tagged-test 0ni1i5d [ cat bites ],,,
    INFO, tagged-test 07cs7hy [ dog moans ], , /matrix-project.qml:27, The dog moans.
    PASS, tagged-test 07cs7hy [ dog moans ],,,


Matrix in :cpp:class:`Project` and in :cpp:class:`Testcase`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A |Matrix| can be defined on two levels: on project level and on test case
level. When defined in project scope, the :cpp:member:`testcases` property
specifies, to which test cases the |Matrix| applies. When defined inside a
:cpp:class:`Testcase` item, the |Matrix| implicitly applies to the surrounding
test case only.

..  code-block:: qml
    :caption: Matrix applies to multiple test cases.

    import qst 1.0

    Project {
        Matrix { ... }
        Testcase { ... }
        Testcase { ... }
        Testcase { ... }
        Testcase { ... }
    }

..  code-block:: qml
    :caption: Matrix applies to single test case.

    import qst 1.0

    Testcase {
        Matrix { ... }
    }

Multiple matrices and overlap
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is possible to define multiple |Matrix| components both on
:cpp:class:`Project` and :cpp:class:`Testcase` level and it is even allowed for
different matrices to attach their tags to the same test cases. However, the
resulting tag combinations must be distinct and must not shadow each other so
that every job can be clearly identified by the combination of test case name
and tags.

..  code-block:: qml
    :caption: ambiguously-overlapping-matrices.qml:
              :code:`Testcase { color: "red" }` would map to 3 different jobs.

    // Matrices shadow each other.
    Matrix {
        testcases: "*"
        Dimension {
            color: [ "red", "blue" ]
        }
    }

    Matrix {
        testcases: "*"
        Dimension {
            color: [ "red", "blue" ]
        }
        Dimension {
            speed: [ "fast", "slow" ]
        }
    }

..  code-block:: qml
    :caption: unambiguously-overlapping-matrices.qml:

    // Matrices are compatible.
    Matrix {
        testcases: "*"
        Dimension {
            color: [ "orange", "green" ]
        }
    }

    Matrix {
        testcases: "*"
        Dimension {
            color: [ "red", "blue" ]
        }
        Dimension {
            speed: [ "fast", "slow" ]
        }
    }

Properties
----------

..  cpp:member:: stringlist testcases

    :default: empty list

    A list of test case names that the matrix is applied to. Entries may contain
    exact names as well as the wildcard characters ``*`` (match any string) and
    ``?`` (match any character). Duplicate entries are ignored.

    Example::

      testcases : [
          "sometest", // Matches only "sometest"
          "test-*",   // Matches any testcase starting with "test-"
          "tc-??",    // Matches any testcase starting with "tc-" followed
                      // by 2 letters.
      ]

    This property is not writable when ``Matrix`` is defined inside a
    :cpp:class:`Testcase` component as the matrix then implicitly applies to the
    surrounding test case only.
