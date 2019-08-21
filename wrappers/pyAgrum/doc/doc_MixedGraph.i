%feature("docstring") gum::MixedGraph
"
MixedGraph represents a Clique Graph.

MixedGraph() -> MixedGraph
    default constructor

MixedGraph(src) -> MixedGraph
    Parameters:
        * **src** (*pyAgrum.MixedGraph*) --the MixedGraph to copy
"

%feature("docstring") gum::MixedGraph::mixedOrientedPath
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
	 a path from node1 to node2, using edges and/or arcs (following the direction of the arcs)

Raises
------
gum.NotFound
	If no path can be found between the two nodes
"

%feature("docstring") gum::MixedGraph::mixedUnorientedPath
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
	 a path from node1 to node2, using edges and/or arcs (not necessarily following the direction of the arcs)

Raises
------
gum.NotFound
	If no path can be found between the two nodes
"

