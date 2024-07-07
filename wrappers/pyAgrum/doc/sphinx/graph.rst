Graphs manipulation
===================

In aGrUM, graphs are undirected (using edges), directed (using arcs) or mixed (using both arcs and edges). Some other types of graphs are described below. Edges and arcs are represented by pairs of int (nodeId), but these pairs are considered as unordered for edges whereas they are ordered for arcs.

For all types of graphs, nodes are int. If a graph of objects is needed (like :py:class:`pyAgrum.bn.BayesNet`), the objects are mapped to nodeIds.


Edges and Arcs
--------------

Arc
^^^

.. autoclass:: pyAgrum.base.Arc


Edge
^^^^

.. autoclass:: pyAgrum.base.Edge

Directed Graphs
---------------

Digraph
^^^^^^^

.. autoclass:: pyAgrum.base.DiGraph

Directed Acyclic Graph
^^^^^^^^^^^^^^^^^^^^^^

.. autoclass:: pyAgrum.base.DAG

Undirected Graphs
-----------------

UndiGraph
^^^^^^^^^

.. autoclass:: pyAgrum.base.UndiGraph

Clique Graph
^^^^^^^^^^^^

.. autoclass:: pyAgrum.base.CliqueGraph

Mixed Graph
-----------

.. autoclass:: pyAgrum.base.MixedGraph

Partially Directed Graph (DAG)
------------------------------

.. autoclass:: pyAgrum.base.PDAG
