Graphs manipulation
===================

In aGrUM, graphs are undirected (using edges), directed (using arcs) or mixed (using both arcs and edges). Some other types of graphs are described below. Edges and arcs are represented by pairs of int (nodeId), but these pairs are considered as unordered for edges whereas they are ordered for arcs.

For all types of graphs, nodes are int. If a graph of objects is needed (like :py:class:`pyagrum.BayesNet`), the objects are mapped to nodeIds.


Edges and Arcs
--------------

Arc
^^^

.. autoclass:: pyagrum.Arc


Edge
^^^^

.. autoclass:: pyagrum.Edge

Directed Graphs
---------------

Digraph
^^^^^^^

.. autoclass:: pyagrum.DiGraph

Directed Acyclic Graph
^^^^^^^^^^^^^^^^^^^^^^

.. autoclass:: pyagrum.DAG

Undirected Graphs
-----------------

UndiGraph
^^^^^^^^^

.. autoclass:: pyagrum.UndiGraph

Clique Graph
^^^^^^^^^^^^

.. autoclass:: pyagrum.CliqueGraph

Mixed Graph
-----------

.. autoclass:: pyagrum.MixedGraph

Partially Directed Graph (DAG)
------------------------------

.. autoclass:: pyagrum.PDAG
