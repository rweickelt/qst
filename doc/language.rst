.. _QML language: https://en.wikipedia.org/wiki/QML
.. _QML language reference: http://doc.qt.io/qt-5/qmlreference.html

Language reference
==================

Qst makes use of the `QML language`_, a declarative language mixed with
Javascript. The official `QML language reference`_ eplains all core concepts in
detail. In difference to a regular QML interpreter, Qst analyses the the
documents before it executes them and makes additions to the QML runtime
environment. It expects a certain structure and enforces some rules. The purpose
of this document is, to eplain all differences.


Context properties
------------------

The following properties are automatically attached by the Qst QML engine to
every loaded documented unless unless otherwise stated.

..  cpp:var:: string path

    Absolute path of the directory where the current file is located.

..  cpp:var:: var profile

    Reference to the global profile object if a :ref:`profile
    <usage_profiles>` is set from command line.

..  cpp:var:: Project project

    Reference to the global project object.

..  cpp:var:: Testcase test

    Reference to the owner test case. This property is only available in
    stand-alone component definitions such as shown in the :ref:`MakeProbe`
    example. It is not available in :cpp:class:`Project` components.


QML data types
--------------

QML types for properties used in this reference documentation. Basic QML types
are also explained in the `QML language reference`_.

..  cpp domain doesn't allow to describe built-in types. But we want to be
..  somehow complete here.

..  c:type:: bool

    Boolean value. Can be either ``true`` or ``false``.

..  c:type:: double

    64 bit (double precision) `floating point
    <https://en.wikipedia.org/wiki/IEEE_754>`_ number.

..  c:type:: int

    32 bit signed whole number.

..  cpp:type:: list

    List of QML objects. See also http://doc.qt.io/qt-5/qml-list.html

..  cpp:type:: signal

    A reference to a `signal attribute
    <http://doc.qt.io/qt-5/qtqml-syntax-objectattributes.html#signal-attributes>`_
    of a component. It is not a dedicated data type, but rather stored as
    :cpp:type:`var`. ``signal`` objects provide a ``connect()`` and
    ``disconnect()`` method to connect to a destination function. They are also
    `functors <https://en.wikipedia.org/wiki/Function_object>`_ and can be
    called like a function in order to invoke connected signal handlers.

..  cpp:type:: string

    Free form text string. See also http://doc.qt.io/qt-5/qml-string.html

..  cpp:type:: var

    Generic property type. Can hold any other data type, but usually used for
    objects only. See also http://doc.qt.io/qt-5/qml-var.html

