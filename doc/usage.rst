.. |Project| replace:: :cpp:class:`Project`
.. |QstS| replace:: :cpp:class:`Qst`
.. |Testcase| replace:: :cpp:class:`Testcase`
.. |run()| replace:: :cpp:func:`run() <Testcase::run()>`

.. _QML language: https://en.wikipedia.org/wiki/QML
.. _QML language reference: http://doc.qt.io/qt-5/qmlreference.html

Usage
=====

This document explains most features of Qst and shows how to use them. It is a
complete walk-through and will take about 15 minutes.

..  contents::
    :local:

Qst makes use of the `QML language`_, a declarative language mixed with
Javascript. The language is very intuitive, but you might want to have a look at
the `QML language reference`_ or keep it open in a separate browser tab.

We will use the terms `item` and `component` a lot. An `item` refers to a data
type whereas `component` usually refers to a specific instance of an item.


Running a simple test case
--------------------------

Let us now get our hands dirty and have a look at a trivial test case. Every
test case is defined by a |Testcase| component and is characterized by at least
2 things:

- a :cpp:member:`name <Testcase::name>` that serves as an identifier,

- a |run()| function which implements sequential test steps.

The name must be unique across the whole project and must be a plain string. The
|run()| function may have arbitrary execution length and contain blocking and
non-blocking function calls. The |Testcase| component is put into a file of an
arbitrary name and in its simplest form it would look as follows:

..  literalinclude:: code/usage/simple-passing-test.qml
    :caption: `simple-passing-test.qml`
    :start-after: header-end

The above test case does not do anything useful. It specifies a name and
contains an empty ``run()`` function. This is equal to a test case without any
failure, so it will simply pass. We will now execute `simple-passing-test.qml`
by typing the following command in a terminal:

..  code-block:: console

    $ qst run --file simple-passing-test.qml
    PASS, simpletest,,,

Qst prints a line of comma-separated values containing the test
:cpp:member:`result <Testcase::result>` and the :cpp:member:`name
<Testcase::name>` of the test case. We will discuss the remaining commas in a
second.

Now let's make the test case fail. For that purpose, Qst offers a couple of
helper :ref:`services <services>` of which |QstS| with its verification
functions :cpp:func:`Qst::compare()` and :cpp:func:`Qst::verify()` is the most
important one:

..  literalinclude:: code/usage/simple-failing-test.qml
    :caption: `simple-failing-test.qml`
    :linenos:
    :start-after: header-end

We execute the test, using a slightly shorter command line than above:

..  code-block:: console

    $ qst run -f simple-failing-test.qml
    FAIL, simple-failing-test, simple-failing-test, simple-failing-test.qml:7, verify() failed

Qst outputs test results in a comma-separated line with the following elements:

- :cpp:member:`result <Testcase::result>` of the test case,
- :cpp:member:`name <Testcase::name>` of the test case,
- name of the current **active component** (on fail only),
- file **location** of the failure (on fail only),
- error **message** (on fail only)

We know the :cpp:member:`result <Testcase::result>` and the :cpp:member:`name
<Testcase::name>` already from the `simple-passing-test.qml` example. The other
elements are only shown when a test case does not pass. In our very simple
example, the test case is the only `active component`. More complex test cases
might form a component tree structure, hence the 3rd column. The `current
location` is the caller position of a verification function. The `message` in
the last column comes from the failing verification function and is either set
by hand or automatically created.

Now we know how to set up a test case and how to run it. Still, it doesn't do
anything useful. We need to connect it to the outside world and fill it with
life.


Adding probe components
-----------------------

Probes connect Qst to the environment, for instance to the file system and other
programs on the computer or even to :doc:`external hardware <hardware-probes>`.
Probes are not a special language construct, but pure QML components with
properties, methods, signals and slots.

A versatile probe item is the :cpp:class:`ProcessProbe`. It can invoke programs
on the local computer and monitor their execution. A very simple test case that
runs GNU Make on a Makefile in the current folder would look like this:

..  literalinclude:: code/usage/makefile-test-simple.qml
    :name: makefile-test-simple
    :caption: `makefile-test-simple.qml`

This example demonstrates the following new elements:

- It instantiates a nested component of the type :cpp:class:`ProcessProbe`
  and assigns custom values to its properties.

- The nested process probe is assigned an
  `id <https://doc.qt.io/qt-5/qtqml-syntax-objectattributes.html#the-id-attribute>`_
  attribute so that it can be referenced.

- The global :cpp:var:`path` context property is used which always points the
  physical directory of the current file.

Various other probe items exist. For a complete list, have a look at the
:doc:`reference <reference>` documentation.


Extending and re-using components
---------------------------------

The above test case :ref:`makefile-test-simple` is not re-usable in this form
because all parameters are hard-coded. Consider a project with a bunch of
makefiles. It would be cumbersome to re-write the whole test case for each
makefile. Instead, we can turn the test case into a re-usable component by
defining additional properties:

..  literalinclude:: code/usage/MakefileTestcase.qml
    :caption: `MakefileTestcase.qml`
    :emphasize-lines: 5-7
    :name: MakefileTestcase

When saving it to a file `MakefileTestcase.qml`, it is automatically available
as component type ``MakefileTestcase`` in other qml files located in the same
directory. Based upon `MakefileTestcase.qml`, we can now create multiple
testcase files in the following form:

..  literalinclude:: code/usage/test-app-build.qml
    :caption: `test-app-build.qml`

..  literalinclude:: code/usage/test-lib-build.qml
    :caption: `test-lib-build.qml`

Later we will learn a way to :ref:`define multiple test case in one document
<tutorial_inline-testcases>`.

Custom properties are also helpful in large test cases. Instead of hard-coding
parameters everywhere in-line, it is better to to put them upfront to make the
test case more readable. This applies to all components in general and is
`common practise in QML <https://doc.qt.io/qt-5/qml-codingconventions.html>`_.


.. _signal-handlers:

Attaching signal handlers
-------------------------

In many probes and test cases we might observe ``onXXX: {...}`` constructs, for
instance::

    Testcase {
        /* ... */
        onCreated: {
            // prepare external resources
        }
    }

Signal handlers are always written in the form `on<CapitalizedSignal>`. `Signals
and signal handlers <http://doc.qt.io/qt-5/qtqml-syntax-signals.html>`_ are a
core concept of the QML language and fall in 1 of 3 categories:

1.  `explicitly defined signals <http://doc.qt.io/qt-5/qtqml-syntax-signals.html#receiving-signals-with-signal-handlers>`_,
    like :cpp:func:`ProcessProbe::finished`,

2.  implicitly defined `property change signals <http://doc.qt.io/qt-5/qtqml-syntax-signals.html#property-change-signal-handlers>`_,

3.  `attached signals <http://doc.qt.io/qt-5/qtqml-syntax-signals.html#attached-signal-handlers>`_
    added by other components.

Signal handlers may contain arbitrary JavaScript code, but they have run-to-
completion semantics and must never do blocking calls. You may follow above
links to read more about this topic. Let us now have a look, how we can utilize
signal handlers in a Qst project.

Our :ref:`MakefileTestcase` file from above can only do one thing: invoke GNU
make. But what if a test case is more complex and needs to invoke additional
programs? In this case, it would be more benefitial to extend
:cpp:class:`ProcessProbe` instead of :cpp:class:`Testcase`:

..  literalinclude:: code/usage/MakeProbe.qml
    :caption: `MakeProbe.qml`
    :name: MakeProbe

The `MakeProbe.qml` component can now be included even multiple times like
this:

..  literalinclude:: code/usage/test-multi-build.qml
    :caption: `test-multi-build.qml`

As we can see in `MakeProbe.qml`, implicit and explicit signal handlers must
be defined in the scope of the signal's owner component. For instance,
``onJobsChanged`` would not work outside ``MakeProbe``. In cases where we
need to handle a signal of a component that is not defined in the current
component, we can use :cpp:class:`SignalProbe` as shown in the
:ref:`signalprobe_ExtendedTestcase` example. It is also possible to use the
`Connections <http://doc.qt.io/qt-5/qtqml-syntax-signals.html#using-the-
connections-type>`_ item from the QtQml package.


Using constraints for continuous evaluation
-------------------------------------------

In the :ref:`MakeProbe` example we have learned how signal handlers can be
used for on-going verification. We do not have to think about them in the
|run()| function, they work silently in the background.

Constraints follow the same principle, but are a bit more formalized and
declarative. They make the test case fail immediately when they are violated.
Constraints are usually connected to signals like the
:cpp:class:`DurationConstraint` or bound to properties like the
:cpp:class:`ValueRangeConstraint`.

Take the following test case as an example:

..  code-block:: qml

    import qst 1.0

    Testcase {
        property int responseTime: 0

        onResponseTimeChanged: {
            Qst.verify((responseTime >= 4) && (responseTime =< 8),
                "responseTime (" + responseTime
                + ") is not in the expected range.")
        }

        function run() {
            // ...
        }
    }

The property ``responseTime`` could be validated by the help of an implicit
property changed signal handler and :cpp:class:`Qst` verification functions. But
we could also use a constraint and improve the readability of the test case:

..  code-block:: qml

    import qst 1.0

    Testcase {
        property int responseTime: 0

        ValueRangeConstraint {
            value: responseTime
            minValue: 4
            maxValue: 8
        }

        function run() {
            // ...
        }
    }



Structuring projects
--------------------

Until now we have only discussed single test cases whereas a real project would
contain many of them. Qst provides the |Project| item for this purpose. A
|Project| component can reference all test cases and serves as the main file of
the project::

    Project {
        name: "referencing-project"
        references: [
            "testcase-file-1.qml",
            "testcase-file-2.qml",
            /* ... */
        ]
    }

.. _tutorial_inline-testcases:

It is also possible to put multiple |Testcase| components into a the same
file by enclosing them in a |Project| component. This is especially useful
when parametrizing and instantiating a generic test case multiple times:

..  code-block:: qml
    :caption: `SpecialTestcase.qml`

    Testcase {
        property int speed

        function run() {
            Qst.info("Speed is set to " + speed)
        }
    }

..  code-block:: qml
    :caption: `project.qml`

    Project {
        name: "inline-project"
        SpecialTestcase { name: "tc-1"; speed: 10 }
        SpecialTestcase { name: "tc-2"; speed: 42000 }
        /* ... */
    }

The |Project| component is automatically attached to any referenced file
and can be accessed as :cpp:var:`project` context property:

..  code-block:: qml
    :caption: `project.qml`

    Project {
        property string host: "http://secr.et"

        references: [
            "test-case-1.qml",
            /* ... */
        ]
    }

..  code-block:: qml
    :caption: `test-case-1.qml`

    Testcase {
        name: "test-case-1"

        function run() {
            Qst.info(name + " is using " + project.host)
        }
    }


Although the :cpp:var:`project` property is shared across the whole project,
test cases are not supposed to write to the project's properties.


.. _usage_profiles:

Working with profiles
---------------------

Qst projects may be developed and executed by multiple developers on different
computers. One usual problem in such setups are differing installation paths,
serial numbers, ports, etc. . Putting machine-dependent values into project
files would only complicate version control and collaboration::

    Testcase {
        property string port: "COM77"   // ttyUSB3 on another computer

        /* ... */
    }

Profiles solve that problem by collecting machine-dependent properties in a JSON
file that remains on the developer's computer:

..  code-block:: json
    :caption: `uart-testing.json on computer 1`

    {
        "port": "COM77",
    }

..  code-block:: json
    :caption: `uart-testing.json on computer 2`

    {
        "port": "ttyUSB3",
    }

The profile name is determined by the file name and selected as command line
option when executing a test project:

..  code-block:: console

    $ qst run --file mytest.qml --profile uart-testing

Profile values can be accessed from anywhere in the project as
:cpp:var:`profile`::

    Testcase {
        property string port: profile.port
    }

Qst tries to load the he selected profile first from the project directory and
if it could not be found, it searches in the »profiles« folder inside the Qst
configuration directory. Additional profile search paths can be specified with
:option:`--profile-directory <run -P>` or :option:`-P <run -P>` respectively:

..  code-block:: console

    $ qst run --f mytest.qml -p uart-testing -P /path/to/profiles

The latter option might be given multiple times.


Storing temporary files
-----------------------

Qst uses a `working directory` to store intermediate files during test
execution. The directory is automatically created in the current folder and by
default it has the format `.<project-name>-<profile-name>-<hash>`. Each test
case will have a sub-folder with its name in the project working directory.

Let us assume a test project that:

#. builds a firmware image,
#. downloads it to the hardware,
#. executes various tests on the hardware.

The directory layout of this imagined example project would look like:

.. code-block:: console

   .myproject-myprofile-e413c0f7
   ├── testcase-build
   │   ├── object-1.o
   │   ├── object-2.o
   │   └── firmware.hex
   ├── testcase-flash
   ├── testcase-some-feature
   └── testcase-other-feature
       └── log.txt


For the ``run`` command, the working directory can be overridden by the
:option:`--working-directory <run -d>` command-line option. When ``run`` is
executed multiple times and the working directory already exists, each
test case sub-folder is wiped out before the :cpp:func:`Testcase::created`
signal is emitted.

