Random Variables
================

aGrUM/pyAgrum is currently dedicated for discrete probability distributions.

There are 3 types of discrete random variables in aGrUM/pyAgrum: LabelizedVariable, DiscretizedVariable and RangeVariable. The 3 types are mainly provided in order to ease modelization. Derived from DiscreteVariable, they share a common API. They essentially differ by the means to create, name and access to their modalities.

Common API for Random Discrete Variables
----------------------------------------
.. autoclass:: pyAgrum.DiscreteVariable

Concrete classes for Random Discrete Variables
----------------------------------------------

LabelizedVariable
^^^^^^^^^^^^^^^^^

.. autoclass:: pyAgrum.LabelizedVariable

DiscretizedVariable
^^^^^^^^^^^^^^^^^^^

.. autoclass:: pyAgrum.DiscretizedVariable

RangeVariable
^^^^^^^^^^^^^

.. autoclass:: pyAgrum.RangeVariable
