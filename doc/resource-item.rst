.. |Resource| replace:: ``Resource``
.. |Testcase| replace:: :qml:item:`Testcase`


Resource Item
=============

..  cpp:class:: Resource

    Abstract entity that synchronizes :cpp:class:`Testcase` execution.

..  cpp:namespace:: Resource

:Locations:
    :qml:item:`Project`

:Nested items:
    none

:Properties:
    :cpp:member:`name`


Detailed Description
--------------------

..  cpp:namespace:: Resource

A |Resource| item is can be acquired and used by |Testcase| items. It may have a
physical counterpart, such as a device or a board, but can be also purely
virtual. Resources in Qst are abstract in the sense that they only represent a
physical resource, but do not provide own runtime functionality. Typically, they
contain configuration data with which the test case initializes its probes.

Resource access is mutual exclusive during testcase execution. That means, only
one |Testcase| item can own and use a resource at the same time. If a testcase
needs a resource, it expresses this using a :qml:item:`Depends` item. If
multiple testcases depend on the same resource, Qst ensures they do not run in
parallel.

..  literalinclude:: code/reference/single-resource.qml
    :caption: single-resource.qml
    :name: single-resource.qml

The example in :ref:`single-resource.qml` contains 3 testcases of which
``test1a`` and ``test1b`` depend on the same resource ``devboard``. `Qst` will
always execute ``test1a`` and ``test1b`` one after the other while ``test2`` is
independent and can run in parallel to the others.

Unlike |Testcase| items, there can be multiple |Resource| items with the same
name in one project. Qst distributes them automatically, ensuring that a maximum
number of testcases are executed simultaneously.


Properties
----------

..  cpp:member:: string name

    :default: empty string

    Identifier used to find the resource with a :qml:item:`Depends` item. This
    property must be a plain string. It is possible to have multiple resource
    items with the same name across the project.
