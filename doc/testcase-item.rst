Testcase Item
=============

Implements a test action.


Properties
----------

- :cpp:member:`Testcase::name`


Signals
-------


Methods
-------

- :cpp:func:`Testcase::run()`


Detailed Description
--------------------

..  cpp:class:: Testcase

    A ``Testcase`` component implements an atomic test action. It may contain
    probes, constraints and other components. The test execution is driven by
    the :cpp:func:`Testcase::run()` function which has to be implemented by the
    user.


..  cpp:member:: string Testcase::name

    :default: empty string

    Identifier for this test case. This property must be a plain string and
    must be unique across the whole project.


..  cpp:function:: void Testcase::run()

    Contains the execution code.