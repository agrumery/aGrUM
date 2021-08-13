%feature("docstring") gum::MixedGraph
"
MixedGraph represents a graph with both arcs and edges.

MixedGraph() -> MixedGraph
    default constructor

MixedGraph(src) -> MixedGraph
    Parameters:
        * **src** (*pyAgrum.MixedGraph*) --the MixedGraph to copy
"
%feature("docstring") gum::MixedGraph::addArc
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
"

%feature("docstring") gum::MixedGraph::addNode
"
Returns
-------
int
  the new NodeId
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
	 a path from node1 to node2, using edges and/or arcs (following the direction of the arcs). If no path is found, the returned list is empty.
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
	 a path from node1 to node2, using edges and/or arcs (not necessarily following the direction of the arcs). If no path is found, the list is empty.

"


%feature("docstring") gum::MixedGraph::addEdge
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
"

%feature("docstring") gum::MixedGraph::addNodeWithId
"
Add a node by choosing a new NodeId.

Parameters
----------
id : int
  The id of the new node

Raises
------
pyAgrum.DuplicateElement
  If the given id is already used
"

%feature("docstring") gum::MixedGraph::addNodes
"
Add n nodes.

Parameters
----------
n : int
  the number of nodes to add.

Returns
-------
Set of int
  the new ids
"

%feature("docstring") gum::MixedGraph::empty
"
Check if the graph is empty.

Returns
-------
bool
    True if the graph is empty
"

%feature("docstring") gum::MixedGraph::emptyEdges
"
Check if the graph doesn't contains edges.

Returns
-------
bool
    True if the graph doesn't contains edges
"

%feature("docstring") gum::MixedGraph::eraseEdge
"
Erase the edge between n1 and n2.

Parameters
----------
n1 : int
  the id of the tail node
n2 : int
  the id of the head node
"

%feature("docstring") gum::MixedGraph::eraseNeighbours
"
Erase all the edges adjacent to a given node.

Parameters
----------
n : int
  the id of the node
"


%feature("docstring") gum::MixedGraph::existsEdge
"
Check if an edge exists bewteen n1 and n2.

Parameters
----------
n1 : int
  the id of one extremity of the edge
n2 : int
  the id of the other extremity if tge edge

Returns
-------
bool
    True if the arc exists
"

%feature("docstring") gum::MixedGraph::existsNode
"
Check if a node with a certain id exists in the graph.

Parameters
----------
id : int
    the checked id

Returns
-------
bool
    True if the node exists
"


%feature("docstring") gum::MixedGraph::size
"
Returns
-------
int
    the number of nodes in the graph
"

%feature("docstring") gum::MixedGraph::sizeEdges
"
Returns
-------
int
    the number of edges in the graph
"

%feature("docstring") gum::MixedGraph::toDot
"
Returns
-------
str
    a friendly display of the graph in DOT format
"


%feature("docstring") gum::MixedGraph::emptyArcs
"
Check if the graph doesn't contains arcs.

Returns
-------
bool
    True if the graph doesn't contains arcs
"

%feature("docstring") gum::MixedGraph::eraseArc
"
Erase the arc between n1 and n2.

Parameters
----------
n1 : int
	the id of the tail node
n2 : int
	the id of the head node
"
%feature("docstring") gum::MixedGraph::eraseChildren
"
Erase the arcs heading through the node's children.

Parameters
----------
n : int
	the id of the parent node
"

%feature("docstring") gum::MixedGraph::eraseNode
"
Erase the node and all the related arcs and edges.

Parameters
----------
id : int
	the id of the node
"

%feature("docstring") gum::MixedGraph::eraseParents
"
Erase the arcs coming to the node.

Parameters
----------
n : int
	the id of the child node
"

%feature("docstring") gum::MixedGraph::existsArc
"
Check if an arc exists bewteen n1 and n2.

Parameters
----------
n1 : int
	the id of the tail node
n2 : int
	the id of the head node

Returns
-------
bool
    True if the arc exists
"

%feature("docstring") gum::MixedGraph::hasDirectedPath
"
Check if a directedpath exists bewteen from and to.

Parameters
----------
from : int
	the id of the first node of the (possible) path
to : int
	the id of the last node of the (possible) path

Returns
-------
bool
    True if the directed path exists
"

%feature("docstring") gum::MixedGraph::sizeArcs
"
Returns
-------
int
    the number of arcs in the graph
"
