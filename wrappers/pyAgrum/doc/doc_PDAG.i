%feature("docstring") gum::PDAG
"
PDAG represents a graph with both arcs and edges.

PDAG() -> PDAG
    default constructor

PDAG(src) -> PDAG
    Parameters:
        * **src** (*pyAgrum.PDAG*) --the PDAG to copy
"
%feature("docstring") gum::PDAG::addArc
"
Add an arc from tail to head.

Parameters
----------
tail : int
  the id of the tail node
head : int
  the id of the head node

Raises
------
  pyAgrum.InvalidNode
    If head or tail does not belong to the graph nodes.

  PyAgrum.InvalidDirectedCycle
    if the arc would create a (mixed) cycle.
"


%feature("docstring") gum::PDAG::mixedOrientedPath
"
Parameters
----------
node1 : int
	the id form which the path begins
node2 : int
	the id to witch the path ends

Returns
-------
List
	 a path from node1 to node2, using edges and/or arcs (following the direction of the arcs). If no path is found, the returned list is empty.
"

%feature("docstring") gum::PDAG::mixedUnorientedPath
"
Parameters
----------
node1 : int
	the id from which the path begins
node2 : int
	the id to which the path ends

Returns
-------
List
	 a path from node1 to node2, using edges and/or arcs (not necessarily following the direction of the arcs). If no path is found, the list is empty.

"


%feature("docstring") gum::PDAG::addEdge
"
Insert a new edge into the graph.

Parameters
----------
n1 : int
  the id of one node of the new inserted edge
n2 : int
  the id of the other node of the new inserted edge

Raises
------
  pyAgrum.InvalidNode
    If n1 or n2 does not belong to the graph nodes.

  PyAgrum.InvalidDirectedCycle
    if the edge would create a (mixed) cycle.
"

%feature("docstring") gum::PDAG::adjacents
"
adjacents nodes are neighbours (not oriented), children and parents

Parameters
----------
id : int
	the id of the node

Returns
-------
set
    the set of node ids.
"
