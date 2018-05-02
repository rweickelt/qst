Component Item
==============

..  cpp:class:: Component

    Base type for various dynamic items such as probes.

..  cpp:namespace:: Component

:Locations:
    :qml:item:`Component`, :qml:item:`Testcase`

:Nested items:
    :qml:item:`Component`,
    `QtObject <http://doc.qt.io/qt-5/qml-qtqml-qtobject.html>`_

:Inherited by:
    :qml:item:`ProcessProbe`, :qml:item:`PinProbe`, :qml:item:`SignalProbe`,
    :qml:item:`UniflashProbe`

:Properties:
    :cpp:member:`name`


Detailed Description
--------------------

..  cpp:namespace:: Component

``Component`` is used as a base type for all kinds of dynamic items in test
cases such as probes and other helpers. ``Component`` items may define their own
properties, methods, signals and signal handlers. All probe items are based upon
``Component``.

Use this item whenever you want to implement re-usable components for the use in
:qml:item:`Testcase` items.

Properties
----------

..  cpp:member:: string name

    :default: typename-number

    Name of the component. When not set, Qst will deduce the final typename and
    add a counter so that the component is assigned a unique name. For instance,
    when implementing `MyProbe.qml` based on ``Component``, the default name for
    probe items will be `myprobe-x` where `x` is a counter value.

    This value does currently only have an effect in error messages.

