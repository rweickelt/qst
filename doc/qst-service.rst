Qst Service
===========

..  cpp:class:: Qst

    Drives test case execution and provides information about the environment.

..  cpp:namespace:: Qst

:Properties:
    :cpp:member:`hostOs`

:Methods:
    :cpp:func:`compare()`, :cpp:func:`error()`, :cpp:func:`info()`,
    :cpp:func:`verify()`, :cpp:func:`wait()`


Detailed Description
--------------------

..  cpp:namespace:: Qst

The ``Qst`` service is an integral part of Qst. It contains methods for test
evaluation evaluation and provides information about test case execution and the
host environment.

``Qst`` is a singleton type and cannot be instantiated as a QML component.


Properties
----------

..  cpp:member:: const string hostOs

    Returns the host operating system that Qst currently runs on.

    ================ =================
    Return value     Operating systems
    ================ =================
    linux            All kinds of Linux distributions on all platforms
    windows          Windows 7, 10
    ================ =================


Methods
-------

..  cpp:function:: void compare(variant actual, variant expected, string message)

    Compares an `actual` value to an `expected` value.  If `actual` and
    `expected` are identical, execution continues. If not, a failure is recorded
    in the test log and the test won't be executed further. The string `message`
    is optional. If not set, a default message is created.

    Both values may be of any basic QML basic type. Arrays are compared by their
    containing values. Objects are compared by each property.

    When ``compare()`` fails during test case execution, the current
    :cpp:class:`Testcase` aborts immediately, the result is set to
    :cpp:enumerator:`Testcase::Result::Fail` and Qst proceeds with the next test
    case. When failing before the first test case starts, for instance in a
    :cpp:class:`Project` item, Qst exits and no test case will be executed at
    all.


..  cpp:function:: void error(string message, string file, int line)

    Throws an error with `message` and aborts the current test case.

    The parameters `file` and `line` are optional. If omitted, Qst
    takes those from the top of the caller stack.


..  cpp:function:: void info(variant message)

    Prints `message` in the test log output. The parameter `message` can be
    anything that converts implicitly into a string.


..  cpp:function:: void verify(bool condition, string message)

    Checks whether the `condition` is true or not. If it is true, execution
    continues. If not, a failure is recorded in the test log and the test won't
    be executed further.

    When ``verify()`` fails during test case execution, the current
    :cpp:class:`Testcase` aborts immediately, the result is set to
    :cpp:enumerator:`Testcase::Result::Fail` and Qst proceeds with the next test
    case. When failing before the first test case starts, for instance in a
    :cpp:class:`Project` item, Qst exits and no test case will be executed at
    all.


..  cpp:function:: void wait(int ms)

    Waits for `ms` milliseconds. While waiting, events will be processed and the
    test stays responsive.

    This function should be only used inside :cpp:func:`Testcase::run()`. It is
    not recommended to call it in binding expressions.
