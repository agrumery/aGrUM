%ignore gum::MixedGraph::addArc;
%ignore gum::MixedGraph::addEdge;
%ignore gum::MixedGraph::addNode;
%ignore gum::MixedGraph::addNodeWithId;
%ignore gum::MixedGraph::empty;
%ignore gum::MixedGraph::emptyArcs;
%ignore gum::MixedGraph::emptyEdges;
%ignore gum::MixedGraph::eraseArc;
%ignore gum::MixedGraph::eraseChildren;
%ignore gum::MixedGraph::eraseEdge;
%ignore gum::MixedGraph::eraseNeighbours;
%ignore gum::MixedGraph::eraseParents;
%ignore gum::MixedGraph::existsArc;
%ignore gum::MixedGraph::existsEdge;
%ignore gum::MixedGraph::existsNode;
%ignore gum::MixedGraph::size;
%ignore gum::MixedGraph::sizeArcs;
%ignore gum::MixedGraph::sizeEdges;

%feature("docstring") gum::MixedGraph
"
MixedGraph represents a Clique Graph.

Available constructors:
    ``MixedGraph() -> MixedGraph``

    ``MixedGraph(src) -> MixedGraph``

Parameters
----------
src : :class: MixedGraph
  the MixedGraph to copy
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

Warnings
--------
NotFound raised if no path can be found between the two nodes
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

Warnings
--------
NotFound raised if no path can be found between the two nodes
"

