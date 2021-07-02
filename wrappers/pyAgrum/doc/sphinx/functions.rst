Functions from pyAgrum
======================

Useful functions in pyAgrum
----------------------------------------------------------------

.. autofunction:: pyAgrum.about

.. autofunction:: pyAgrum.getPosterior

Quick specification of (randomly parameterized) graphical models
----------------------------------------------------------------

aGrUM/pyAgrum offers a compact syntax that allows to quickly specify prototypes of graphical models. These `fastPrototype` aGrUM's methods have also been wrapped in functions of pyAgrum.

.. code:: python

    gum.fastBN("A->B<-C;B->D")

The type of the random variables can be specifiy with different syntaxes:

      - by default, a variable is a :class:`pyAgrum.RangeVariable` using the default domain size (second argument of the functions).
      - with ``a[10]``, the variable is a :class:`pyAgrum.RangeVariable` using 10 as domain size (from 0 to 9)
      - with ``a[3,7]``, the variable is a :class:`pyAgrum.RangeVariable` using a domainSize from 3 to 7
      - with ``a[1,3.14,5,6.2]``, the variable is a :class:`pyAgrum.DiscretizedVariable` using the given ticks (at least 3 values)
      - with ``a{top|middle|bottom}``, the variable is a :class:`pyAgrum.LabelizedVariable` using the given labels (here : 'top', 'middle' and 'bottom').
      - with ``a{-1|5|0|3}``, the variable is a :class:`pyAgrum.IntegerVariable` using the sorted given values.

.. note::
  - If the dot-like string contains such a specification more than once for a variable, the first specification will be used.
  - the CPTs are randomly generated.

.. autofunction:: pyAgrum.fastBN

.. autofunction:: pyAgrum.fastMN

.. autofunction:: pyAgrum.fastID

Input/Output for Bayesian networks
----------------------------------

.. autofunction:: pyAgrum.availableBNExts

.. autofunction:: pyAgrum.loadBN

.. autofunction:: pyAgrum.saveBN

Input/Output for Markov networks
----------------------------------

.. autofunction:: pyAgrum.availableMNExts

.. autofunction:: pyAgrum.loadMN

.. autofunction:: pyAgrum.saveMN

Input for influence diagram
----------------------------------

.. autofunction:: pyAgrum.availableIDExts

.. autofunction:: pyAgrum.loadID

.. autofunction:: pyAgrum.saveID
