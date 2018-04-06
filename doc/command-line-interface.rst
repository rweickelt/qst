Command-line interface
======================

Qst provides multiple command-line commands. They can be invoked as `qst
<command name>`. The following commands are available:

..  contents::
    :local:
    :depth: 1

run
---

..  program:: run

Executes a test case or a test project and outputs the results.

Synopsis
~~~~~~~~

..  code-block:: console

    qst run [options]

Description
~~~~~~~~~~~

The ``run`` command executes a Qst `.qml` file containing a
:cpp:class:`Testcase` or a :cpp:class:`Project` component. The file is specified
by the :option:`-f` option.


Options
~~~~~~~

..  option:: -d, --working-directory <path>

    Use `<path>` as working directory during test case execution. If the
    directory does not exist, it is being created. By default, a directory with
    the name `.<project-name>-<profile-name>-<hash>` is created in the current
    path.

..  option:: -f, --file <file>

    Use `<file>` as project file. If `<file>` is a directory and contains a
    single file ending in `.qml`, that file will be used. If this option is not
    given at all, the behavior is the same as for ``--file ./``.

..  option:: -h, --help

    Displays this help.

..  option:: -I, --import <path>

    Add `<path>` as a directory where the engine searches for installed modules
    in a URL-based directory structure.

    The path may be a local file system directory, a Qt Resource path
    (:/imports), a Qt Resource url (qrc:/imports) or a URL.

..  option:: -p, --profile <name>

    Load a :ref:`profile <usage_profiles>` from `<name>.json` and attach it to
    the QML root context. Matching profiles are searched in the directories
    specified by :option:`-P` or in the project directory or in the config
    directory's profile sub-directory  (in this order).


..  option:: -P, --profile-directory <path>

    When looking for :ref:`profiles <usage_profiles>`, search first in `<path>`.
    This option might be given multiple times.


version
-------

Prints the version of Qst to stdout.

Synopsis
~~~~~~~~

..  code-block:: console

    qst version

Description
~~~~~~~~~~~

Prints the version of Qst to stdout.

Options
~~~~~~~

This command takes no options.