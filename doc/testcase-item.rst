Testcase Item
=============

..  cpp:class:: Testcase

    Implements a test action.

..  cpp:namespace:: Testcase

:Locations:
    :qml:item:`Project`, document root

:Nested items:
    :qml:item:`Component`, :qml:item:`Matrix`

:Enumerations:
    :cpp:enum:`Result`

:Properties:
    :cpp:member:`elapsedTime`, :cpp:member:`name`, :cpp:member:`result`,
    :cpp:member:`workingDirectory`

:Methods:
    :cpp:func:`run()`,

:Signals:
    :cpp:func:`created()`, :cpp:func:`destruction()`, :cpp:func:`finished()`,
    :cpp:func:`started()`


Detailed Description
--------------------

..  cpp:namespace:: Testcase

A `Testcase` component combines probes, constraints and other QML components
into a single test case. It is executed as one atomic entity. Every test case
must have a unique name throughout the project and must implement a
:cpp:func:`run()` method.

Example::

    import qst 1.0

    Testcase {
        name: "test-myprocess"

        ProcessProbe {
            id: process
            command: "/usr/bin/echo"
            arguments: [
                "Hello World"
            ]
        }

        function run() {
            process.start();
            process.waitForFinished(100);
            Qst.compare(process.readAllStandardOutput(), "Hello World");
            // Testcase will finish with result 'Pass'
        }
    }


Enumerations
------------

..  cpp:enum:: Result

    The outcome of the test case execution. The default value after start is
    :cpp:enumerator:`Unfinished`. The result changes to :cpp:enumerator:`Fail`
    when the first evaluation statement fails or otherwise it is set to
    :cpp:enumerator:`Pass` when the :cpp:func:`run()` method completes without
    any issue.

    ..  cpp:enumerator:: Unfinished

        Default value after start.

    ..  cpp:enumerator:: Pass

        The method :cpp:func:`run()` has finished without any issue.

    ..  cpp:enumerator:: Fail

        An error occured or one of the constraints has failed while
        executing :cpp:func:`run()`.


Properties
----------

..  cpp:member:: uint64 elapsedTime

    Time in milliseconds since the test case was started.

    This property is updated on every read access and cannot be used as a
    binding.


..  cpp:member:: string name

    Identifier for this test case. This property must be a plain string and
    must be unique across the whole project.

    The default value is an empty string.


..  cpp:member:: Result result

    The outcome of a test case run. The default value is
    :cpp:enumerator:`Result::Unfinished`.


..  cpp:member:: const string workingDirectory

    A unique directory where the current test case and child components may
    store files. The directory may be used for log files or as scratch pad and
    is always created on execution start.

    The default value is ``project.workingDirectory + "/" + name`` and cannot be
    changed by the test case.


Methods
-------

..  cpp:function:: void run()

    Contains the execution code and is called when the test case starts. It has
    to be implemented by the user. The ``run()`` method may contain several
    other blocking calls to methods and functions. It will return only for two
    reasons:

    1.  All statements have been executed. :cpp:member:`result`
        will be set to :cpp:enumerator:`Result::Pass`.

    2.  An error has occurred or a constraint has failed and
        :cpp:member:`result` has been set to :cpp:enumerator:`Result::Fail`.
        In that case, all remaining statements are aborted.

Signals
-------

..  cpp:function:: void created()

    This signal is emitted after the :cpp:func:`destruction()` signal of the
    previous ``Testcase`` and before :cpp:func:`started()` of the current one.
    It is the QML way of implementing a constructor and can be used to
    initialize resources.

    The signal is also attached to every :cpp:class:`Component` as
    ``Testcase.created()``.


..  cpp:function:: void destruction()

    This signal is emitted before the ``Testcase`` component is physically
    destroyed and may be used to free up resources.

    The signal is also attached to every :cpp:class:`Component` as
    ``Testcase.destruction()``.


..  cpp:function:: void finished()

    This signal is emitted after the :cpp:func:`run()` function has returned and
    may be used for final evaluation or cleaning up. By that time,
    :cpp:member:`result` may not have been set, yet and thus, it is still
    allowed to call verification functions.

    The signal is also attached to every :cpp:class:`Component` as
    ``Testcase.finished()``.


..  cpp:function:: void started()

    This signal is emitted after :cpp:func:`created()` and before
    :cpp:func:`run()` is invoked. It may be used for starting concurrent
    activities.

    The signal is also attached to every :cpp:class:`Component` as
    ``Testcase.started()``.

