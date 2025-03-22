Random Variables
================

aGrUM/pyAgrum is currently dedicated for discrete probability distributions.

There are 5 types of discrete random variables in aGrUM/pyAgrum: :class:`pyagrum.LabelizedVariable`, :class:`pyagrum.DiscretizedVariable`, :class:`pyagrum.IntegerVariable`, :class:`pyagrum.RangeVariable` and :class:`pyagrum.NumericalDiscreteVariable`. The 5 types are mainly provided in order to ease modelization. Derived from DiscreteVariable, they share a common API. They essentially differ by the means to create, name and access to their modalities.

The function :meth:`pyagrum.fastVariable` allows to easily create variables of any types with the *fast* syntax.

Common API for Random Discrete Variables
----------------------------------------
.. autoclass:: pyagrum.DiscreteVariable

Concrete classes for Random Discrete Variables
----------------------------------------------

LabelizedVariable
^^^^^^^^^^^^^^^^^

.. autoclass:: pyagrum.LabelizedVariable

DiscretizedVariable
^^^^^^^^^^^^^^^^^^^

.. autoclass:: pyagrum.DiscretizedVariable

IntegerVariable
^^^^^^^^^^^^^^^

.. autoclass:: pyagrum.IntegerVariable

RangeVariable
^^^^^^^^^^^^^

.. autoclass:: pyagrum.RangeVariable

NumericalDiscreteVariable
^^^^^^^^^^^^^^^^^^^^^^^^^

.. autoclass:: pyagrum.NumericalDiscreteVariable
