Exports Item
============

..  cpp:class:: Exports

    Forwards data to dependent :class:`Testcase` items.

..  cpp:namespace:: Exports

:Locations:
    :cpp:class:`Testcase`


Detailed Description
--------------------

..  cpp:namespace:: Exports

The `Exports` item helps the surrounding test case to forward results and
additional data to dependent test cases. It requires a `Depends` item on the
other side to access the forwarded results. Together, the `Exports` and the
`Depends` item form a dataflow graph. The `Exports` item does not have any
built-in properties. Here is an example:

..  literalinclude:: code/reference/simple-exports.qml
    :caption: `simple-exports.qml`
    :emphasize-lines: 8-11,14

After completion of `A`, Qst copies the properties of the :qml:item:`Exports`
item over to `B`. Test case `B` can access them through the
:qml:prop:`dependencies` property. Only
:ref:`basic QML types <language_basic-data-types>` are considered for exporting
which avoids side effects.