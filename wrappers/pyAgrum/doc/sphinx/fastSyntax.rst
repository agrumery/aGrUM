*fast* syntax
==============

Quick specification of discrete random variables
------------------------------------------------

aGrUM/pyAgrum offers a so-called *fast* syntax that allows to quickly and compactly specify prototypes of graphical models.

The type of the (discrete) random variables can be specifiy with different syntaxes:

      - by default, a variable is a :class:`pyAgrum.RangeVariable` using the default domain size (second argument of the functions).
      - with ``a[10]``, the variable is a :class:`pyAgrum.RangeVariable` using 10 as domain size (from 0 to 9)
      - with ``a[3,7]``, the variable is a :class:`pyAgrum.RangeVariable` using the integer values from 3 to 7.
      - with ``a[1,3.14,5,6.2]``, the variable is a :class:`pyAgrum.DiscretizedVariable` using the given ticks (at least 3 values).
      - with ``a[1:6.15:9]``, the variable is a :class:`pyAgrum.DiscretizedVariable` using the ticks from 1 to 6.15 in 9 steps (the domain size is 9).
      - with ``a+[1,3.14,5,6.2]``, the variable is a :class:`pyAgrum.DiscretizedVariable` using the given ticks (at least 3 values), empirical.
      - with ``a+[1:6.15:9]``, the variable is a :class:`pyAgrum.DiscretizedVariable` using the ticks from 1 to 6.15 in 9 steps (the domain size is 9), empirical.
      - with ``a{top|middle|bottom}``, the variable is a :class:`pyAgrum.LabelizedVariable` using the given labels (here : `top`, `middle` and `bottom`).
      - with ``a{-1|5|0|3}``, the variable is a :class:`pyAgrum.IntegerVariable` using the sorted given values.
      - with ``a{-0.5|5.01|0|3.1415}``, the variable is a :class:`pyAgrum.NumericalDiscreteVariable` using the sorted given values.
      - with ``a{-0.5:3.1415:5}``, the variable is a :class:`pyAgrum.NumericalDiscreteVariable` using the values from -0.5 to 3.1415 in 5 steps (the domain size is 5).

.. autofunction:: pyAgrum.fastVariable

Quick specification of (randomly parameterized) graphical models
----------------------------------------------------------------

These `fastPrototype` aGrUM's methods have also been wrapped in functions of pyAgrum.

.. code:: python

    pyAgrum.fastBN("A[10]->B<-C{top|middle|bottom};B->D")

.. note::
  - If the dot-like string contains such a specification more than once for a variable, the first specification will be used.
  - the CPTs are randomly generated.

.. autofunction:: pyAgrum.fastBN

.. autofunction:: pyAgrum.fastMRF

.. autofunction:: pyAgrum.fastID
