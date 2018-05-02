ProcessProbe Item
=================

..  cpp:class:: ProcessProbe

    Starts and watches processes.

..  cpp:namespace:: ProcessProbe

:Inherits:
    :qml:item:`Component`

:Enumerations:
    :cpp:enum:`ProcessError`. :cpp:enum:`State`

:Properties:
    :cpp:member:`arguments`, :cpp:member:`exitCode`, :cpp:member:`program`
    :cpp:member:`state`, :cpp:member:`workingDirectory`


:Methods:
    :cpp:func:`readAllStandardError()`, :cpp:func:`readAllStandardOutput()`
    :cpp:func:`start()`, :cpp:func:`terminate()`, :cpp:func:`waitForFinished()`
    :cpp:func:`waitForStarted()`

:Signals:
    :cpp:func:`errorOccurred()`, :cpp:func:`started()`, :cpp:func:`finished()`


Detailed Description
--------------------

..  cpp:namespace:: ProcessProbe

``ProcessProbe`` can invoke external applications and communicate with them. It
mirrors the API of `QProcess <http://doc.qt.io/qt-5/qprocess.html>`_.

The process to run is defined by the :cpp:member:`program` property. Arguments
are provided as a string list in the :cpp:member:`arguments` property.

Example::

    import qst 1.0

    Testcase {

        ProcessProbe {
            id: make
            program : (Qst.hostOs === "windows") ? "gmake.exe" : "make"
            arguments: [
                "-j",
                "8"
            ]
        }

        function run() {
            make.start();
            make.waitForFinished(1701);
            Qst.compare(make.exitCode, 0, "Make did not succeed");
        }
    }



Enumerations
------------


..  cpp:enum:: ProcessError

    Error events during process execution. Further information may be obtained
    with :cpp:func:`errorString()`.

    ..  cpp:enumerator:: Crashed

        The process crashed some time after starting successfully.

    ..  cpp:enumerator:: FailedToStart

        The process failed to start. Either the invoked program is missing, or
        you may have insufficient permissions to invoke the program.

    ..  cpp:enumerator:: ReadError

        An error occurred when attempting to read from the process. For example,
        the process may not be running.

    ..  cpp:enumerator:: Timedout

        The last waitFor...() function timed out. The state of ``ProcessProbe``
        is unchanged, and you can try calling waitFor...() again.

    ..  cpp:enumerator:: WriteError

        An error occurred when attempting to write to the process. For example,
        the process may not be running, or it may have closed its input channel.

    ..  cpp:enumerator:: UnknownError

        An unknown error occurred.


..  cpp:enum:: State

    Execution state of the process.

    ..  cpp:enumerator:: NotRunning

        The process has not been started or it has already finished.

    ..  cpp:enumerator:: Starting

        The process is starting, but it is not running yet.

    ..  cpp:enumerator:: Running

        The process is running.


Properties
----------

..  cpp:member:: stringlist arguments

    Process arguments in list format. Unlike on a shell, spaces do not separate
    arguments and thus, each argument must be a separate list entry. Escaping
    is required for arguments containing spaces.::

        // Wrong
        arguments: [ "--file My\ File.txt " ]

        // Correct
        arguments: [
            "--file",
            "My File.txt"
        ]


..  cpp:member:: integer exitCode

    The exit code of the last process that finished. This property is
    read-only.


..  cpp:member:: string program

    Path to an executable to run.


..  cpp:member:: State state

    The current execution state of the process.


..  cpp:member:: string workingDirectory


Methods
-------

..  cpp:function:: string errorString()

    Returns a human-readable description of the last error that occurred.


..  cpp:function:: string readAllStandardError()

    This function returns all data available from the standard error of the
    process.


..  cpp:function:: string readAllStandardOutput()

    This function returns all data available from the standard output of the
    process.


..  cpp:function:: void start()

    Starts the process set by :cpp:member:`program` with the arguments given by
    :cpp:member:`arguments`. This function returns immediately.


..  cpp:function:: void terminate()

    Attempts to terminate the process.


..  cpp:function:: bool waitForFinished(int milliseconds)

    Blocks until the process has finished and the :cpp:func:`finished()` signal
    has been emitted, or until `milliseconds` have passed.

    Returns ``true`` if the process finished; otherwise returns ``false`` (if
    the operation timed out, if an error occurred, or if this process is
    already finished).

    If `milliseconds` is -1, this function will not time out.


..  cpp:function:: bool waitForStarted(int milliseconds)

    Blocks until the process has started and the :cpp:func:`started()` signal
    has been emitted, or until `milliseconds` have passed.

    Returns ``true`` if the process was started successfully; otherwise returns
    ``false`` (if the operation timed out or if an error occurred).


Signals
-------

..  cpp:function:: void errorOccurred(ProcessError error)

    An error has occurred during execution. A human-readable version of the
    error may be obtained with :cpp:func:`errorString()`.


..  cpp:function:: void finished()

    This signal is emitted when the process finishes. :cpp:member:`exitCode` is
    the exit code of the process (only valid for normal exits). After the
    process has finished, the buffers in QProcess are still intact. You can
    still read any data that the process may have written before it finished.


..  cpp:function:: void started()

    This signal is emitted by ``ProcessProbe`` when the process has started, and
    :cpp:member:`state` is :cpp:enumerator:`State::Running`.
