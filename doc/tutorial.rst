Qst in a nutshell
=================

This document explains the basic features of Qst and shows how to use them.

Defining test cases
-------------------

- test case item
- probe items
- run function + Qst.verify, Qst.compare


Running test cases
------------------

- command line example
- working directory
- looking at the output


Concurrent execution and evaluation
-----------------------------------

- add event handlers to probes
- adding constraints to do permanent checks


Working with profiles
---------------------

Qst projects may be developed and executed by multiple developers on different
computers. One usual problem in such setups are differing installation paths,
serial numbers, ports, etc. . Putting machine-dependent values into project
files would only complicate version control and collaboration::

    Testcase {
        property string port: COM77     // ttyUSB3 on another computer

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

Profile values can be accessed from anywhere in the project as ``profile``::

    Testcase {
        property string port: profile.port
    }

Qst tries to load the he selected profile first from the project directory and
if it could not be found, it searches in the `profiles` folder inside the Qst
configuration directory. Additional profile search paths can be specified with
``--profile-directory`` or ``-P`` respectively:

..  code-block:: console

    $ qst run --f mytest.qml -p uart-testing -P /path/to/profiles

The latter option might be given multiple times.
