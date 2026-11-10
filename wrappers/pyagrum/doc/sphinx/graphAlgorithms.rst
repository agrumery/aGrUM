Algorithms on graphs
====================

The module :mod:`pyagrum.graph` gathers the algorithms on graphs: one function per algorithm, whatever the
kind of graph. It is imported explicitly:

.. code-block:: python

  import pyagrum.graph as gg

A worked example: from a DAG to its Markov equivalence class and back.

.. code-block:: python

  import pyagrum as gum
  import pyagrum.graph as gg

  dag = gum.fastDAG("A->B->C;D->C;C->E")
  gg.topologicalOrder(dag)
  # [3, 0, 1, 2, 4]
  gg.ancestors(dag, "C")
  # {0, 1, 3}

  # the class of the DAG: its CPDAG, a graph with arcs (compelled) and edges (reversible)
  cpdag = gg.cpdag(dag)
  sorted(cpdag.arcs()), sorted(cpdag.edges())
  # ([(1, 2), (2, 4), (3, 2)], [(0, 1)])
  gg.isChordal(cpdag)  # the chain components of a CPDAG are chordal
  # True

  # the same d-separations in the DAG and in its class
  gg.separation(dag, "A", "D"), gg.separation(cpdag, "A", "D")
  # (True, True)
  gg.separation(dag, "A", "D", "E"), gg.separation(cpdag, "A", "D", "E")
  # (False, False)

  # back to a DAG of the class
  sorted(gg.consistentExtension(cpdag).arcs())
  # [(0, 1), (1, 2), (2, 4), (3, 2)]

.. pyagrum-functions:: pyagrum.graph
