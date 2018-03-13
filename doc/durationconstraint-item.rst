DurationConstraint Item
=======================

..  cpp:class:: DurationConstraint

    Checks the time between two signal occurrences.

..  cpp:namespace:: DurationConstraint

..  rubric:: Properties

- :cpp:member:`beginOn`
- :cpp:member:`duration`
- :cpp:member:`enabled`
- :cpp:member:`endOn`
- :cpp:member:`evaluateOnFinished`
- :cpp:member:`evaluateOnValidation`
- :cpp:member:`maxDuration`
- :cpp:member:`minDuration`
- :cpp:member:`timedOut`
- :cpp:member:`valid`


..  rubric:: Methods:

- :cpp:func:`begin()`
- :cpp:func:`end()`


Detailed Description
--------------------

..  cpp:namespace:: DurationConstraint

``DurationConstraint`` measures the time between two signals
:cpp:member:`beginOn` and :cpp:member:`endOn`. The methods :cpp:func:`begin()`
or :cpp:func:`end()` may be used as an alternative if no signals are available.
The expected duration is configured with :cpp:member:`minDuration` and
:cpp:member:`maxDuration`.

The constraint has two stages: validation and evaluation. Validation happens
immediately after the  :cpp:member:`endOn` signal has fired. In the same moment,
the :cpp:member:`duration` property is updated and shows the measured duration.
Whether the duration is within the expected range, can be seen at the property
:cpp:member:`valid`.

During the evaluation stage, it is decided whether the test case is aborted.
Evaluation can either happen immediately after validation
(:cpp:member:`evaluateOnValidation` is ``true``) or just before the
:cpp:func:`Testcase::finished` event (:cpp:member:`evaluateOnFinished` is
``true``). The :cpp:member:`enabled` property switches measurement completely on
and off.


Example for measuring the duration between two signals::

    import qst 1.0

    Testcase {

        ProcessProbe {
            id: process
            command: "sleep"
            arguments: [ 3 ]
        }

        DurationConstraint {
            id: constraint
            minDuration: 2.9
            maxDuration: 3.1

            beginOn: process.started
            endOn: process.finished
        }

        function run() {
            process.start()
            Qst.wait(4000)
        }


Example for monitoring a sequence of actions::

    import qst 1.0

    Testcase {

        DurationConstraint {
            id: constraint
            minDuration: 5
            maxDuration: 10
        }

        function run() {
            constraint.begin()

            // Now do something very time consuming...
            Qst.wait(20); // will be aborted because constraint
                          // is violated after 10 ms.
        }



Properties
----------

..  cpp:member:: signal beginOn

    :default: undefined

    Starts a measurement action. This property must either be QML signal or an
    object that defines a ``connect()`` method with a signal handler as
    parameter. When updating this property, the old signal will be disconnected.

    See also: :cpp:member:`endOn`, :cpp:func:`begin()`, :cpp:func:`end()`


..  cpp:member:: double duration

    :default: 0.0

    The measured timed between :cpp:member:`beginOn` and :cpp:member:`endOn`.


..  cpp:member:: bool enabled

    :default: ``true``

    When ``false``, the signals :cpp:member:`beginOn` and :cpp:member:`endOn` do
    not have any effect and the constraint is neither validated nor evaluated.


..  cpp:member:: signal endOn

    :default: undefined

    Ends a measurement cycle and triggers validation. This property must either
    be QML signal or an object that defines a ``connect()`` method with a signal
    handler as parameter. When updating this property, the old signal will be
    disconnected.

    See also: :cpp:member:`beginOn`, :cpp:func:`begin()`, :cpp:func:`end()`


..  cpp:member:: bool evaluateOnFinished

    :default: false

    If ``true``, the constraint will be evaluated by `Qst` just before
    :cpp:func:`Testcase::finished()`.

    See also :cpp:member:`evaluateOnValidation`


..  cpp:member:: bool evaluateOnValidation

    :default: true

    If ``true``, the test case will fail immediately when validation fails.  If
    ``false``, then the constraint will not be evaluated immediately. Instead,
    it will be evaluated on :cpp:func:`Testcase::finished()`.

    See also :cpp:member:`evaluateOnFinished`


..  cpp:member:: double maxDuration

    :default: 0.0

    Specifies the maximum allowed time between :cpp:member:`beginOn` and
    :cpp:member:`endOn`. The value must be greater or equal
    :cpp:member:`minDuration`.


    See also :cpp:member:`minDuration`


..  cpp:member:: double minDuration

    :default: 0.0

    Specifies the minimum allowed time between :cpp:member:`beginOn` and
    :cpp:member:`endOn`. The value must be less or equal
    :cpp:member:`maxDuration`.

    See also :cpp:member:`maxDuration`


..  cpp:member:: bool timedOut

    :default: false

    Becomes ``true`` when the time between :cpp:member:`beginOn` and
    :cpp:member:`endOn` exceeds the specified duration range. This property is
    set back to ``false`` on each measurement interval.


..  cpp:member:: bool valid

    :default: false

    Reflects whether :cpp:member:`duration` is within the specified range.



Methods
-------

..  cpp:function:: void begin()

    Equivalent to :cpp:member:`beginOn`. Can be used to start a measurement
    manually or when no signal is available to attach to.

    See also: :cpp:func:`end()`, :cpp:member:`beginOn`, :cpp:member:`endOn`


..  cpp:function:: void end()

    Equivalent to :cpp:member:`beginOn`. Can be used to start a measurement
    manually or when no signal is available to attach to.

    See also: :cpp:func:`begin()`, :cpp:member:`beginOn`, :cpp:member:`endOn`