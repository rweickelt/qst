CounterConstraint Item
======================

..  cpp:class:: CounterConstraint

    Checks properties and signals against a threshold.


..  cpp:namespace:: CounterConstraint

..  rubric:: Properties

- :cpp:member:`count`
- :cpp:member:`enabled`
- :cpp:member:`maxCount`
- :cpp:member:`property`
- :cpp:member:`signal`
- :cpp:member:`valid`
- :cpp:member:`validateImmediately`


..  rubric:: Methods:

- :cpp:func:`clear()`


Detailed Description
--------------------

..  cpp:namespace:: CounterConstraint

`CounterConstraint` can be used in two ways:

1. To bind to an integer :cpp:member:`property` of another component and check
   that property against :cpp:member:`maxCount`.

2. To bind to a :cpp:member:`signal` of another component, count the trigger
   amount and check that against :cpp:member:`maxCount`.

In both cases, :cpp:member:`count` is always implicitly updated and contains the
most recent value. If :cpp:member:`count` exceeds :cpp:member:`maxCount`,
:cpp:member:`valid` is set to ``false``.

The evaluation behavior can be influenced with
:cpp:member:`validateImmediately`.

Example for binding to a signal::

    Testcase {
        QtQml.Timer {
            id : timer
            interval: 5
            periodic: true
        }

        CounterConstraint {
            id: constraint
            signal: timer.triggered
            maxCount: 2
        }

        function run() {
            timer.start();
            Qst.wait(20); // will not be reached because the constraint is
                          // is violated after 15 ms
        }


Example for binding to a property::

    Testcase {
        property int myCount: 0

        CounterConstraint {
            id: constraint
            property: myCount
            maxCount: 2
        }

        function run() {
            // Test case will aborted after 2 loop iterations
            for (int i = 0; i < 47; i++) {
                // Do something intensive
                myCount++
            }
        }


Properties
----------

..  cpp:member:: int count

    :default: 0

    Reflects either the number of times that :cpp:member:`signal` has been
    emitted or reflects the value of :cpp:member:`property`. This property is
    always implicitly updated.


..  cpp:member:: bool enabled

    :default: ``true``

    When ``false``, updates of :cpp:member:`count` do not have any effect and
    the constraint is never evaluated.


..  cpp:member:: int maxCount

    :default: 0

    Specifies the maximum :cpp:member:`count` for :cpp:member:`valid` being
    ``true``.

..  cpp:member:: int property

    :default: 0

    An integer property of another item. This property must be a QML property
    allowing binding creation and thus, emitting a ``propertyChanged()`` signal.
    When updating this property during run-time, a proper binding must be
    created::

        Testcase {

            property int someProperty
            property int anotherProperty

            CounterConstraint {
                id: constraint
                property: someProperty
            }

            function run() {
                // Correct
                constraint.property = Qt.binding(function(){ return anotherProperty; })

                // Wrong
                constraint.property = anotherProperty
            }
        }

    See also: `Creating property bindings from JavaScript <http://doc.qt.io/qt-5/qtqml-syntax-propertybinding.html#creating-property-bindings-from-javascript>`_


..  cpp:member:: Signal signal

    :default: undefined

    A signal of another item. This property must either be QML signal or an
    object that defines a ``connect()`` method with a signal handler as
    parameter. When updating this property, the old signal will be disconnected.


..  cpp:member:: bool valid

    :default: true

    Reflects whether :cpp:member:`count` is less or equal
    :cpp:member:`maxCount`. This property is always updated and not influenced
    by :cpp:member:`validateImmediately`.


..  cpp:member:: bool validateImmediately

    :default: true

    If ``true``, the test case will fail immediately when :cpp:member:`count`
    exceeds :cpp:member:`maxCount`. If ``false``, then the constraint will not
    be evaluated immediately. Instead, it will be evaluated on
    :cpp:func:`Testcase::finished()`.


Methods
-------

..  cpp:function:: void clear()

    Resets :cpp:member:`count` to 0 and :cpp:member:`valid` to ``true``.