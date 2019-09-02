Graphs manipulation
===================

In aGrUM, graphs are undirected (using edges), directed (using arcs) or mixed (using both arcs and edges). Some other types of graphs are described below. Edges and arcs are represented by pairs of int (nodeId), but these pairs are considered as unordered for edges whereas they are ordered for arcs.

For all types of graphs, nodes are int. If a graph of objects is needed (like :py:class:`pyAgrum.BayesNet`), the objects are mapped to nodeIds.

Edges and Arcs
--------------

Arc
^^^

.. autoclass:: pyAgrum.Arc

Edge
^^^^

.. autoclass:: pyAgrum.Edge

Directed Graphs
---------------

Digraph
^^^^^^^

.. autoclass:: pyAgrum.DiGraph

Directed Acyclic Graph
^^^^^^^^^^^^^^^^^^^^^^

.. autoclass:: pyAgrum.DAG

Undirected Graphs
-----------------

UndiGraph
^^^^^^^^^

.. autoclass:: pyAgrum.UndiGraph

Clique Graph
^^^^^^^^^^^^

.. autoclass:: pyAgrum.CliqueGraph

Mixed Graph
-----------

.. autoclass:: pyAgrum.MixedGraph

