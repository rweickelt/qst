Depends Item
============

..  cpp:class:: Depends

    Describes dependencies between :class:`Testcase` items.

..  cpp:namespace:: Depends

:Locations:
    :cpp:class:`Testcase`

:Properties:
    :qml:prop:`alias`, :qml:prop:`name`

Detailed Description
--------------------

..  cpp:namespace:: Depends

The ``Depends`` defines a dependency on another test case specified by
:qml:prop:`name`. In its simplest form, it specifies only a precedence relation
as for instance in the following example:

..  literalinclude:: code/reference/simple-depends.qml
    :caption: `simple-depends-exports.qml`
    :emphasize-lines: 15


Accessing properties of dependencies
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In the above example, test case `B` may want to access results and properties of
`A`. The `Depends` item attaches a reference of selected properties from `A` to
`B`. These properties are available in `B` through the
:qml:prop:`Testcase::dependencies` property as shown in the following example:

..  literalinclude:: code/reference/simple-exports.qml
    :caption: `simple-exports.qml`
    :emphasize-lines: 21-23,26

After completion of `A`, Qst copies the properties of the :qml:item:`Exports`
item over to `B`. Test case `B` can access them through the
:qml:prop:`dependencies` property. Only
:ref:`basic QML types <language_basic-data-types>` are considered for exporting
which avoids side effects.


Dependencies and tags
~~~~~~~~~~~~~~~~~~~~~

When being assigned to a :qml:item:`Matrix` item, a testcase may be expanded to
multiple jobs. The `Depends` item allows to control which job instances of one
test case ``A`` match which job instances of a test case ``B``. It comes in two
matching flavours. Consider the following ``Matrix`` project:

..  literalinclude:: code/reference/depends-project-with-matrix.qml
    :caption: `depends-project-with-matrix.qml`
    :name: depends-project-with-matrix

The project in `depends-project-with-matrix.qml` consists of 3 test cases, 2 of
which depend on a test case ``A``. The referenced test cases will be explained
in the following sub-sections.


Explicit matching
#################

The `Depends` item specifies **all** tag keys of the depending test case. This
matching type is always unambiguous and is especially useful when a non-tagged
test case depends only on certain jobs of a tagged one. Example:

..  literalinclude:: code/reference/explicit.qml
    :caption: `explicit.qml`
    :name: explicit
    :emphasize-lines: 7-14

..  code-block:: console

    $ qst run -f depends-project-with-matrix.qml | grep -e explicit -e " A "
    PASS, A 0000000 [ board-1 debug ],,,
    PASS, A 0000001 [ board-2 debug ],,,
    PASS, A 0000002 [ board-1 release ],,,
    PASS, A 0000003 [ board-2 release ],,,
    INFO, explicit, , explicit.qml:21, depends on board-1-debug.elf board-2-debug.elf
    PASS, explicit,,,


Implicit matching
#################

The `Depends` item specifies **none** of the tags of a depending test case. In
this case, the matching behavior depends on whether the test case and the
dependency are tagged or not. Non-tagged example:

..  literalinclude:: code/reference/implicit.qml
    :caption: `implicit.qml` The test case implicit (not tagged) depends on
              ``A`` (tagged).
    :name: implicit
    :emphasize-lines: 10-12


..  code-block:: console

    $ qst run -f depends-project-with-matrix.qml | grep -e implicit -e " A "
    PASS, A 0000000 [ board-1 debug ],,,
    PASS, A 0000001 [ board-2 debug ],,,
    PASS, A 0000002 [ board-1 release ],,,
    PASS, A 0000003 [ board-2 release ],,,
    INFO, implicit 0000004 [ board-1 debug ],, implicit.qml:19, depends on board-1-debug.elf
    PASS, implicit 0000004 [ board-1 debug ],,,
    INFO, implicit 0000005 [ board-2 debug ],, implicit.qml:19, depends on board-2-debug.elf
    PASS, implicit 0000005 [ board-2 debug ],,,
    INFO, implicit 0000006 [ board-1 release ],, implicit.qml:19, depends on board-1-release.elf
    PASS, implicit 0000006 [ board-1 release ],,,
    INFO, implicit 0000007 [ board-2 release ],, implicit.qml:19, depends on board-2-release.elf
    PASS, implicit 0000007 [ board-2 release ],,,


:``B`` (tagged) depends on ``A`` (tagged):

  Every job instance of ``B`` matches all jobs
  of ``A`` that have at least the same tags as
  ``B``. If ``A`` and ``B`` are in the same matrix,
  then the resulting relationship is usually 1:1.

:``B`` (not tagged) depends on ``A`` (tagged):

  There is only a single instance of ``B`` and
  it matches all instances of ``A``.

:``B`` (tagged) depends on ``A`` (not tagged):

  Every job instance of ``B`` matches the
  same single instance of ``A``.

:``B`` (not tagged) depends on ``A`` (not tagged):

  There is only a single instance ``A`` and ``B``.


Properties
----------

..  cpp:member:: string alias

    The :qml:prop:`name` may not be valid JavaScript identifier or multiple
    `Depends` items may reference the same :qml:prop:`Testcase::name`. In this
    case, `alias` specifies the identifier under which exported values can be
    accessed through the :qml:prop:`Testcase::dependencies` property.

    This property must be a plain string and a valid JavaScipt identifer.

..  cpp:member:: string name

    The name of the depending test case. It must match with an existing
    :qml:prop:`Testcase::name`.

    This property must be a plain string and cannot be empty.