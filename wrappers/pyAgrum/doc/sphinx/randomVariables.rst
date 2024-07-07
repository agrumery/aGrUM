Random Variables
================

aGrUM/pyAgrum is currently dedicated for discrete probability distributions.

There are 5 types of discrete random variables in aGrUM/pyAgrum: :class:`pyAgrum.base.LabelizedVariable`, :class:`pyAgrum.base.DiscretizedVariable`, :class:`pyAgrum.base.IntegerVariable`, :class:`pyAgrum.base.RangeVariable` and :class:`pyAgrum.base.NumericalDiscreteVariable`. The 5 types are mainly provided in order to ease modelization. Derived from DiscreteVariable, they share a common API. They essentially differ by the means to create, name and access to their modalities.

The function :meth:`pyAgrum.base.fastVariable` allows to easily create variables of any types with the *fast* syntax.

Common API for Random Discrete Variables
----------------------------------------
.. autoclass:: pyAgrum.base.DiscreteVariable

Concrete classes for Random Discrete Variables
----------------------------------------------

LabelizedVariable
^^^^^^^^^^^^^^^^^

.. autoclass:: pyAgrum.base.LabelizedVariable

DiscretizedVariable
^^^^^^^^^^^^^^^^^^^

.. autoclass:: pyAgrum.base.DiscretizedVariable

IntegerVariable
^^^^^^^^^^^^^^^

.. autoclass:: pyAgrum.base.IntegerVariable

RangeVariable
^^^^^^^^^^^^^

.. autoclass:: pyAgrum.base.RangeVariable

NumericalDiscreteVariable
^^^^^^^^^^^^^^^^^^^^^^^^^

.. autoclass:: pyAgrum.base.NumericalDiscreteVariable
