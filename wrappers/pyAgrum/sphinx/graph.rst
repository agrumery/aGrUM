Graphs manipulation
===================

In aGrUM, graphs are undirected (with edges), directed (with arcs) or mixed. Some other types of graphs are described below. Edges and arcs are represented by pairs of int (nodeId).

For all types of graphs, nodes are int. If a graph of objects is needed (like :py:class:`pyAgrum.BayesNet`), the objects are mapped to nodeIds.

Edges and Arcs
--------------
.. autoclass:: pyAgrum.Arc
  :members:

.. autoclass:: pyAgrum.Edge
  :members:

Graphs
------
.. autoclass:: pyAgrum.DiGraph
  :members:

.. autoclass:: pyAgrum.UndiGraph
  :members:

.. autoclass:: pyAgrum.MixedGraph
  :members:

.. autoclass:: pyAgrum.DAG
  :members:

.. autoclass:: pyAgrum.CliqueGraph
  :members:
