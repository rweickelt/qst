
[![Build Status](https://travis-ci.org/rweickelt/qst.svg?branch=master)](https://travis-ci.org/rweickelt/qst)
[![Documentation Status](https://readthedocs.org/projects/qst/badge/?version=latest)](http://qst.readthedocs.io/en/latest/?badge=latest)


Qst
===

Qst (pronounced /kwɛst/) is a firmware testing automation suite.


Key features
------------

- **Declarative markup language (QML)**

  Write readable test cases that are easy to understand and to maintain.
  Encapsulate complex functionality in components and re-use them without
  duplicating code.

- **Concurrent execution**

  Express concurrent operations and complex constraints in addition to a
  traditional sequential check-marking approach. This speeds up test execution
  and allows ongoing validity checks.

- **Separate functionality from configuration**

  Develop test cases on your local computer and execute them on other machines
  and different target hardware without modifying the test code. Keep the
  project configuration separate from the project sources by specifying
  profiles.


The combination of these features makes Qst a compelling alternative to any
purely script-based solution.


Documentation
-------------

User documentation is available at https://qst.readthedocs.org


Building
--------

It is recommended to use Docker images for building. Use the following
instructions if you still want to build in a conventional environment.

<h3>Host application (Qst)</h3>

Use the Docker images available at https://github.com/rweickelt/docker-qt or
alternatively:

1.  Prerequisites:

    - [Qt framework](https://qt.io) >= 5.9
    - [Qt Build Suite](https://qbs.io) >= 1.9.1 or [QtCreator](https://www.qt.io/) >= 4.4

2.  A Qbs build profile for your Qt installation should have been automatically
    created by Qt Creator. If you are using Qbs stand-alone, have a look at the
    [Qbs documentation](http://doc.qt.io/qbs/qt-versions.html).

3.  Build and install. Run also regression tests.

    ```
    qbs install --install-root /opt/qst --file qst-project.qbs \
    profile:myqtprofile project.runAutotest:true
    ```

<h3>Launchpad probe application</h3>

Use the Docker image described in the [Dockerfile](docker/Dockerfile) or
alternatively:

1.  Prerequisites:

    - [SimpleLink CC13x0 SDK](http://www.ti.com/tool/SIMPLELINK-CC13X0-SDK) >= 1.50.00
    - [Uniflash](http://www.ti.com/tool/UNIFLASH) >= 4.2
    - [GCC ARM toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)
    - [Qt Build Suite](https://qbs.io) >= 1.9.1 or [QtCreator](https://www.qt.io/) >= 4.4

2.  Create a Qbs build profile for the ARM toolchain:

    ```
    qbs setup-toolchains --type gcc /path/to/compiler gcc_arm_6
    qbs config profiles.gcc_arm_6.qbs.architecture arm
    qbs config profiles.gcc_arm_6.cpp.compilerName g++
    qbs config profiles.gcc_arm_6.cpp.cxxCompilerName g++
    ```

3.  Based on the toolchain profile, create another profile for the SimpleLink SDK
    that you want to use.

    ```
    qbs config profiles.simplelink_cc13x0.baseProfile gcc_arm_6
    qbs config profiles.simplelink_cc13x0.simplelink.core.deviceFamily cc13x0
    qbs config profiles.simplelink_cc13x0.simplelink.core.installPath /opt/ti/${SDK}
    qbs config profiles.simplelink_cc13x0.simplelink.tirtos.xdcInstallPath /opt/ti/${XDC}
    qbs config profiles.simplelink_cc13x0.cpp.platformCommonCompilerFlags "[\"-mcpu=cortex-m3\", \"-mthumb\", \"-mfloat-abi=soft\"]"
    ```

4. Build and install the launchpad probe application:

   ```
   qbs install --install-root /opt/qst --file qst-project.qbs profile:simplelink-cc13x0-1.60
   ```


License
-------

Licensed under GPLv3. See http://github.com/rweickelt/qst/COPYING for details.