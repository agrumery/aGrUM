Random Variables
================

aGrUM/pyAgrum is currently dedicated for discrete probability distributions.

There are 5 types of discrete random variables in aGrUM/pyAgrum: :class:`pyAgrum.LabelizedVariable`, :class:`pyAgrum.DiscretizedVariable`, :class:`pyAgrum.IntegerVariable`, :class:`pyAgrum.RangeVariable` and :class:`pyAgrum.NumericalDiscreteVariable`. The 5 types are mainly provided in order to ease modelization. Derived from DiscreteVariable, they share a common API. They essentially differ by the means to create, name and access to their modalities.

The function :meth:`pyAgrum.fastVariable` allows to easily create variables of any types with the *fast* syntax.

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

IntegerVariable
^^^^^^^^^^^^^^^

.. autoclass:: pyAgrum.IntegerVariable

RangeVariable
^^^^^^^^^^^^^

.. autoclass:: pyAgrum.RangeVariable

NumericalDiscreteVariable
^^^^^^^^^^^^^^^^^^^^^^^^^

.. autoclass:: pyAgrum.NumericalDiscreteVariable
