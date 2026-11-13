*fast* syntax
==============

Quick specification of discrete random variables
------------------------------------------------

aGrUM/pyAgrum offers a so-called *fast* syntax that allows to quickly and compactly specify prototypes of graphical models.

The type of the (discrete) random variables can be specifiy with different syntaxes:

      - by default, a variable is a :class:`pyagrum.RangeVariable` using the default domain size (second argument of the functions).
      - with ``a[10]``, the variable is a :class:`pyagrum.RangeVariable` using 10 as domain size (from 0 to 9)
      - with ``a[3,7]``, the variable is a :class:`pyagrum.RangeVariable` using the integer values from 3 to 7.
      - with ``a[1,3.14,5,6.2]``, the variable is a :class:`pyagrum.DiscretizedVariable` using the given ticks (at least 3 values).
      - with ``a[1:6.15:9]``, the variable is a :class:`pyagrum.DiscretizedVariable` using the ticks from 1 to 6.15 in 9 steps (the domain size is 9).
      - with ``a+[1,3.14,5,6.2]``, the variable is a :class:`pyagrum.DiscretizedVariable` using the given ticks (at least 3 values), empirical.
      - with ``a+[1:6.15:9]``, the variable is a :class:`pyagrum.DiscretizedVariable` using the ticks from 1 to 6.15 in 9 steps (the domain size is 9), empirical.
      - with ``a{top|middle|bottom}``, the variable is a :class:`pyagrum.LabelizedVariable` using the given labels (here : ``top``, ``middle`` and ``bottom``).
      - with ``a{-1|5|0|3}``, the variable is a :class:`pyagrum.IntegerVariable` using the sorted given values.
      - with ``a{-0.5|5.01|0|3.1415}``, the variable is a :class:`pyagrum.NumericalDiscreteVariable` using the sorted given values.
      - with ``a{-0.5:3.1415:5}``, the variable is a :class:`pyagrum.NumericalDiscreteVariable` using the values from -0.5 to 3.1415 in 5 steps (the domain size is 5).

.. pyagrum-functions::

   pyagrum.fastVariable

.. _quoted-names-in-fast-syntax:

Names between quotes
--------------------

A name may be written between single quotes, in every *fast* syntax (variables, graphical models and graphs): every
character is then part of the name, a doubled quote standing for a quote.

.. code:: python

    pyagrum.fastBN("'Visit Asia'{yes|no}->Dyspnoea<-'X-ray'[3];'l''eau'")
    pyagrum.fastID("'my state'->*'my decision'->$'my utility'")
    pyagrum.fastMRF("'a--b'--C")
    pyagrum.fastDiGraph("'Visit Asia'->X")
    pyagrum.fastPAG("'A'o->'B'")

- A quote opens a quoted name only at the beginning of a name: ``O'Brien->X`` and the labels ``{l'eau|x}`` are
  unchanged.
- Without quotes, the name of a variable may contain spaces (``Visit Asia->X``), but not ``-``, ``;``, ``[``, ``{``...
  The name of a node of a graph contains neither space, ``'<'`` nor ``'>'``.
- In a quoted name, ``'12'`` is a name, not a node id; ``'X+'[0,1,2]`` is not empirical, ``'X'+[0,1,2]`` is.
- ``toFast()`` quotes a name only if it would not be read back as is otherwise (see `pyagrum.quoteFastName`).

.. warning::
  The files saved by pyAgrum (``.jgum``, ``.bgum``, ``.bifxml``, pickle...) write the variables in *fast* syntax. A
  name which needs quotes, such as ``X-ray`` (or the names ``X[0]`` of the variables of a `pyagrum.ktbn.KTBN`), is
  written ``'X-ray'[3]``: pyAgrum 3.3.2 or older read it with the quotes in the name. The simple
  names are written as before.

.. pyagrum-functions::

   pyagrum.quoteFastName
   pyagrum.fastNameNeedsQuotes

.. _quick-specification-of-randomly-parameterized-graphical-models:

Quick specification of (randomly parameterized) graphical models
----------------------------------------------------------------

Using the *fast* syntax, one can quickly specify a Bayesian network (arcs ``->``), an influence diagram (arcs, with the prefixes ``*`` and ``$`` for the decision and utility nodes) or a Markov random field (the variables of a factor separated by ``--``). The *fast* syntax specifies the variables and the structure of the graphical model, not its parameters (CPTs, factors, utilities): they are randomly generated. When a node appears several times, its first occurrence gives its variable and, in an influence diagram, its kind.

.. code:: python

    pyagrum.fastBN("A[10]->B<-C{top|middle|bottom};B->D")

.. note::
  - If the dot-like string contains such a specification more than once for a variable, the first specification will be used.
  - the CPTs are randomly generated.

.. pyagrum-functions::

   pyagrum.fastBN
   pyagrum.fastMRF
   pyagrum.fastID

Checking a description
----------------------

A description may be valid for several *fast* syntaxes: ``'A->B'`` is a directed, a mixed and a marked graph, a
Bayesian network and an influence diagram, ``'A--B'`` only a Markov random field. `pyagrum.checkFastSyntax` gives the verdict for every syntax
(this is how `pyagrum.fastGraph` chooses the type of graph it builds).

.. code:: python

    >>> pyagrum.checkFastSyntax("A{yes|no}->Visit Asia").validSyntaxes()
    [<FastSyntax.BayesNet: 4>, <FastSyntax.InfluenceDiagram: 5>]

.. pyagrum-functions::

   pyagrum.checkFastSyntax

.. pyagrum-class:: pyagrum.FastSyntaxCheck

.. pyagrum-class:: pyagrum.FastSyntax
