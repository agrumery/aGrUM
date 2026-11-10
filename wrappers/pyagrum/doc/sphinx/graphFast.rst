Quick specification of graphs
=============================

Graphs can also be built with a *fast* dot-like syntax, similar to the one used for graphical
models (see :ref:`quick-specification-of-randomly-parameterized-graphical-models`) : ``'->'``
denotes a directed arc, ``'<-'`` a directed arc in the other direction, ``'-'`` an undirected
edge, ``';'`` separates independent chains.

.. code:: python

    pyagrum.fastDiGraph('A->B->C;B->E')
    pyagrum.fastDiGraph('A->B<-C')       # both A and C point to B
    pyagrum.fastUndiGraph('A-B-C')
    pyagrum.fastMixedGraph('A->B-C')
    pyagrum.fastDAG('A->B->C')
    pyagrum.fastPDAG('A->B-C')
    pyagrum.fastChainGraph('A->B-C')
    pyagrum.fastMarkedGraph('A->B;B<->C;C o-> D;D o-o E;E-F')
    pyagrum.fastPAG('X o-o Y;Y o-> Z')

.. note::
  A single ``'-'`` (not ``'--'``) is used for edges on purpose : :func:`pyagrum.fastMRF` already
  uses ``'--'`` to list the variables of a single factor (a clique), a different construct from
  a chain of pairwise edges. Writing ``'A--B'`` in these functions raises an error rather than
  being silently misread.

.. pyagrum-functions::

   pyagrum.fastDiGraph
   pyagrum.fastUndiGraph
   pyagrum.fastMixedGraph
   pyagrum.fastDAG
   pyagrum.fastPDAG
   pyagrum.fastChainGraph
   pyagrum.fastMarkedGraph
   pyagrum.fastPAG

See also this `notebook <https://pyagrum.readthedocs.io/en/latest/notebooks/93-Tools_tensorsAndGraphs.html>`_
for a tour of graph construction and algorithms.
