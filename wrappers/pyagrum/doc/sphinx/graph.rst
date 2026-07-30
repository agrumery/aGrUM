Graphs manipulation
===================

In aGrUM, graphs are undirected (using edges), directed (using arcs) or mixed (using both arcs and edges). Some other types of graphs are described below. Edges and arcs are represented by pairs of int (nodeId), but these pairs are considered as unordered for edges whereas they are ordered for arcs.

For all types of graphs, nodes are int. If a graph of objects is needed (like :py:class:`pyagrum.BayesNet`), the objects are mapped to nodeIds.

Quick specification of graphs
------------------------------

Graphs can also be built with a *fast* dot-like syntax, similar to the one used for graphical
models (see :ref:`quick-specification-of-randomly-parameterized-graphical-models`) : ``'->'``
denotes a directed arc, ``'<-'`` a directed arc in the other direction, ``'-'`` an undirected
edge, ``';'`` separates independent chains.

.. code:: python

    pyagrum.fastDiGraph('A->B->C;B->E')
    pyagrum.fastDiGraph('A->B<-C')       # both A and C point to B
    pyagrum.fastUndiGraph('A-B-C')
    pyagrum.fastMixedGraph('A->B-C')

.. note::
  A single ``'-'`` (not ``'--'``) is used for edges on purpose : :func:`pyagrum.fastMRF` already
  uses ``'--'`` to list the variables of a single factor (a clique), a different construct from
  a chain of pairwise edges. Writing ``'A--B'`` in these functions raises an error rather than
  being silently misread.

.. autofunction:: pyagrum.fastDiGraph

.. autofunction:: pyagrum.fastUndiGraph

.. autofunction:: pyagrum.fastMixedGraph

.. autofunction:: pyagrum.fastDAG

.. autofunction:: pyagrum.fastPDAG

See also this `notebook <https://pyagrum.readthedocs.io/en/latest/notebooks/93-Tools_tensorsAndGraphs.html>`_
for a tour of graph construction and algorithms.


Edges and Arcs
--------------

.. autoclass:: pyagrum.Arc

.. autoclass:: pyagrum.Edge

Directed Graphs
---------------

.. autoclass:: pyagrum.DiGraph

.. autoclass:: pyagrum.DAG

Undirected Graphs
-----------------

.. autoclass:: pyagrum.UndiGraph

.. autoclass:: pyagrum.CliqueGraph

Mixed Graph
-----------

.. autoclass:: pyagrum.MixedGraph

Partially Directed Graph (DAG)
------------------------------

.. autoclass:: pyagrum.PDAG

Partial Ancestral Graph (PAG)
------------------------------

.. autoclass:: pyagrum.PAG
