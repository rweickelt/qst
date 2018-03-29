[![Build Status](https://travis-ci.org/rweickelt/qst.svg?branch=master)](https://travis-ci.org/rweickelt/qst)
[![Documentation Status](https://readthedocs.org/projects/qst/badge/?version=latest)](http://qst.readthedocs.io/en/latest/?badge=latest)
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Frweickelt%2Fqst.svg?type=shield)](https://app.fossa.io/projects/git%2Bgithub.com%2Frweickelt%2Fqst?ref=badge_shield)


Qst
===

Qst (pronounced /kwɛst/) is a firmware test automation suite targeting
embedded devices.


Key features
------------

- **Declarative markup language (QML):**
  Write readable test cases that are easy to understand and to maintain.
  Encapsulate complex functionality in components and re-use them
  without duplicating code.

- **Concurrent execution:**
  Express concurrent operations and complex constraints in addition
  to a traditional sequential check-marking approach.
  This speeds up test execution and allows ongoing validity checks.

- **Separate functionality from configuration:**
  Write test cases on one computer and execute them on many
  others without modifying the source code. Run the same test
  case on different target hardware by specifying profiles.

The combination of the above features makes Qst superior to any purely
script-based solution.


Documentation
-------------

User documentation is available at https://qst.readthedocs.org


Building
--------

Host application:

1. Prerequisites:

   - [Qt framework](https://qt.io) >= 5.9
   - [Qt Build Suite](https://qbs.io) >= 1.9.1 or [QtCreator](https://www.qt.io/) >= 4.4

2. A Qbs build profile for your Qt installation should have been automatically created
   by Qt Creator. If you are using Qbs stand-alone, have a look at the
   [Qbs documentation](http://doc.qt.io/qbs/qt-versions.html).

3. Build and install

   ```
   qbs install --install-root /opt/qst --file qst-project.qbs profile:myqtprofile
   ```

Remote probe application (when using a launchpad as a cheap IO interface):

1. Prerequisites:

   - [SimpleLink CC13x0 SDK](http://www.ti.com/tool/SIMPLELINK-CC13X0-SDK)
   - [Uniflash](http://www.ti.com/tool/UNIFLASH) >= 4.2
   - [GCC ARM toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)
   - [Qt Build Suite](https://qbs.io) >= 1.9.1 or [QtCreator](https://www.qt.io/) >= 4.4

2. Create a Qbs build profile for the ARM toolchain:

   ```
   qbs setup-toolchains /path/to/compiler gcc-arm-2017-q2
   ```

3. Based on the toolchain profile, create another profile for the SimpleLink SDK that you
   want to use. You may create multiple different profiles.

   ```
   TBD
   ```

4. Build and install the launchpad probe application:

   ```
   qbs install --install-root /opt/qst --file qst-project.qbs profile:simplelink-cc13x0-1.60
   ```


License
-------

Licensed under GPLv3. See http://github.com/rweickelt/qst/COPYING for details.

## License
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Frweickelt%2Fqst.svg?type=large)](https://app.fossa.io/projects/git%2Bgithub.com%2Frweickelt%2Fqst?ref=badge_large)