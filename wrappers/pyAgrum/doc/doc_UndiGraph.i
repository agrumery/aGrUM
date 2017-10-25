%feature("docstring") gum::UndiGraph
"
UndiGraph represents a Directed Graph.

Available constructors:
    ``UndiGraph() -> UndiGraph``

    ``UndiGraph(src) -> UndiGraph``

Parameters
----------
src : :class: UndiGraph
  the UndiGraph to copy

"

%feature("docstring") gum::UndiGraph::addEdge
"
Insert a new edge into the graph.

Parameters
----------
n1 : int
  the id of one node of the new inserted edge
n2 : int
  the id of the other node of the new inserted edge

Warnings
--------
InvalidNode raised if n1 or n2 does not belong to the graph nodes.
"