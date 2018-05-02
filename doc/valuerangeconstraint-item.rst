ValueRangeConstraint Item
=========================

..  cpp:class:: ValueRangeConstraint

    Checks whether a property is in a certain range.

..  cpp:namespace:: ValueRangeConstraint

:Inherits:
    :qml:item:`Component`

:Properties:
    :cpp:member:`enabled`, :cpp:member:`evaluateOnFinished`,
    :cpp:member:`evaluateOnValidation`, :cpp:member:`maxValue`
    :cpp:member:`minValue`, :cpp:member:`valid`, :cpp:member:`value`


Detailed Description
--------------------

..  cpp:namespace:: ValueRangeConstraint

``ValueRangeConstraint`` checks whether :cpp:member:`value` is within a range
given by  :cpp:member:`minValue` and :cpp:member:`maxValue`.

The constraint has two stages: validation and evaluation. Validation happens
immediately after the  :cpp:member:`value` property has changed.  Whether
:cpp:member:`value` is within the expected range, can be seen at the property
:cpp:member:`valid`.

During the evaluation stage, it is decided whether the test case is aborted.
Evaluation can either happen immediately after validation
(:cpp:member:`evaluateOnValidation` is ``true``) or just before the
:cpp:func:`Testcase::finished` event (:cpp:member:`evaluateOnFinished` is
``true``). The :cpp:member:`enabled` property switches measurement completely on
and off.


Example for measuring the duration between two signals::

    import qst 1.0
    import QtQml 2.0 as QtQml

    Testcase {

        QtQml.Timer {
            id: timer
            repeat: true
            interval: 123

            onTriggered: adc.start
        }

        AdcProbe {
            id: adc
            ioid: 23
        }

        ValueRangeConstraint {
            id: constraint
            minValue: 100
            maxValue: 300

            value: adc.value
        }

        function run() {
            timer.start
            Qst.wait(4000)
        }


Properties
----------

..  cpp:member:: bool enabled

    :default: ``true``

    When ``false``, the :cpp:member:`value` property does not have any effect
    and the constraint is neither validated nor evaluated.


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


..  cpp:member:: var maxValue

    :default: undefined

    Specifies the maximum allowed :cpp:member:`value`.

    See also :cpp:member:`minValue`


..  cpp:member:: var minValue

    :default: undefined

    Specifies the minimum allowed :cpp:member:`value`.

    See also :cpp:member:`maxValue`


..  cpp:member:: bool valid

    :default: false

    Reflects whether :cpp:member:`value` is within the specified range.


..  cpp:member:: var value

    :default: undefined

    The value to be validated. This property is usually bound to a property of
    another component. It may also be written manually if desired.
