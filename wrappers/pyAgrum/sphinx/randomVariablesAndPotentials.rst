Random Variables and Potentials
===============================

aGrUM/pyAgrum is dedicated for discrete probability distribution.

There are 3 types of discrete random variables in aGrUM/pyAgrum : LabelizedVariable, DiscretizedVariable and RangeVariable. The 3 types are mainly provided in order to ease the modelisation. Derived from DiscreteVariable, they share a common API. They essentially differ by the means to create, name and access to their modalities.

Common API for Random Discrete Variables
----------------
.. autoclass:: pyAgrum.DiscreteVariable
  :members:

Concrete classes for Random Discrete Variables
----------------

.. autoclass:: pyAgrum.LabelizedVariable
  :members:

.. autoclass:: pyAgrum.DiscretizedVariable

.. autoclass:: pyAgrum.RangeVariable
  :members:

Instantiation and Potential
---------------------------
.. autoclass:: pyAgrum.Instantiation
  :members:

.. autoclass:: pyAgrum.Potential
  :members:
