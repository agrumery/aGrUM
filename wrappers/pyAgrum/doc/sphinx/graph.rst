Graphs manipulation
===================

In aGrUM, graphs are undirected (with edges), directed (with arcs) or mixed. Some other types of graphs are described below. Edges and arcs are represented by pairs of int (nodeId).

For all types of graphs, nodes are int. If a graph of objects is needed (like :py:class:`pyAgrum.BayesNet`), the objects are mapped to nodeIds.

Edges and Arcs
--------------

Arc
^^^

.. autoclass:: pyAgrum.Arc
  :members:

Edge
^^^^

.. autoclass:: pyAgrum.Edge
  :members:

Directed Graphs
---------------

Digraph
^^^^^^^

.. autoclass:: pyAgrum.DiGraph
  :members:

Directed Acyclic Graph
^^^^^^^^^^^^^^^^^^^^^^
.. autoclass:: pyAgrum.DAG
  :members:

Undirected Graphs
-----------------

UndiGraph
^^^^^^^^^

.. autoclass:: pyAgrum.UndiGraph
  :members:

Clique Graph
^^^^^^^^^^^^

.. autoclass:: pyAgrum.CliqueGraph
  :members:

Mixed Graph
-----------
  .. autoclass:: pyAgrum.MixedGraph
    :members:
