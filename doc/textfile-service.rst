TextFile Service
================

..  cpp:class:: TextFile

    Allows to read from and write into text files.

..  cpp:namespace:: TextFile

..  rubric:: Enumerations

- :cpp:enum:`OpenMode`

..  rubric:: Methods

- :cpp:func:`TextFile::TextFile()`
- :cpp:func:`atEndOfFile()`
- :cpp:func:`close()`
- :cpp:func:`readAll()`
- :cpp:func:`readLine()`
- :cpp:func:`truncate()`
- :cpp:func:`write()`
- :cpp:func:`writeLine()`


Detailed Description
--------------------

..  cpp:namespace:: TextFile

The ``TextFile`` service provides convenient access to text files in Javascript
context. For instance, within :cpp:func:`Testcase::run()`::

    function run() {
        // ...
        var file = new TextFile(test.workingDirectory + "/hello.txt",
                TextFile.WriteOnly, "UTF-8");
        file.write("Hello World");
        file.close();
        // ...
    }

``TextFile`` is to be used in Javascript context only and cannot be instantiated
as a QML component.



Enumerations
------------

..  cpp:enum:: OpenMode

    Specified an access qualifier for the file. Options may be OR'ed togther::

        new TextFile(path, TextFile.ReadWrite | TextFile.Append, "UTF-8");


    ..  cpp:enumerator:: Append

        The file is opened for appending text. This is option
        may be combined with :cpp:enumerator:`WriteOnly`.

    ..  cpp:enumerator:: ReadOnly

        Opens the file for read access only. Several instances may
        open the file in read-only mode at the same time.

    ..  cpp:enumerator:: ReadWrite

        Opens the file for read and write access.

    ..  cpp:enumerator:: WriteOnly

        Opens the file for write access only.


Methods
-------

..  cpp:function:: TextFile(string filePath, OpenMode mode, string codec)

    Constructs a ``TextFile`` object and opens the file `filePath` with `mode`
    access permission and using a text codec specified by `codec`.

    The `filePath` must be an absolute path. Options for `codec` are the same as
    for `QTextCodec <http://doc.qt.io/qt-5/qtextcodec.html#details>`_, for
    instance "UTF-8", "UTF-16", "ISO 8859-1" and others.

    The default value for `mode` is :cpp:enumerator:`OpenMode::ReadWrite`.
    The default value for `codec` is "UTF-8".


..  cpp:function:: bool atEndOfFile()

    Returns ``true`` if no more data can be read from the file, ``false``
    otherwise.


..  cpp:function:: void close()

    Closes the file. It is recommended to always call this function as soon as
    you are finished with the file, in order to keep the number of in-flight
    file descriptors as low as possible.


..  cpp:function:: string readAll()

    Reads all data from the file and returns it.


..  cpp:function:: string readLine()

    Reads one line of text from the file and returns it. The returned string
    does not contain the newline characters.


..  cpp:function:: void truncate()

    Truncates the file. Sets the file size to zero and removes all content.


..  cpp:function:: void write(string data)

    Writes `data` into the file at the current position.


..  cpp:function:: void writeLine(string data)

    Writes `data` into the file at the current position and appends the newline
    character(s).
