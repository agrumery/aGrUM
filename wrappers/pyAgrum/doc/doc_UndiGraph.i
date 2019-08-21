%feature("docstring") gum::UndiGraph
"
UndiGraph represents an Undirected Graph.

UndiGraph() -> UndiGraph

UndiGraph(src) -> UndiGraph
    Parameters!
        * **src** (*UndiGraph*) -- the pyAgrum.UndiGraph to copy

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

Raises
------
gum.InvalidNode
  If n1 or n2 does not belong to the graph nodes.
"

%feature("docstring") gum::UndiGraph::addNode
"
Returns
-------
int
  the new NodeId
"

%feature("docstring") gum::UndiGraph::addNodes
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

%feature("docstring") gum::UndiGraph::addNodeWithId
"
Add a node by choosing a new NodeId.

Parameters
----------
id : int
  The id of the new node

Raises
------
gum.DuplicateElement
  If the given id is already used
"

%feature("docstring") gum::UndiGraph::clear
"
Remove all the nodes and edges from the graph.
"

%feature("docstring") gum::UndiGraph::edges
"
Returns
-------
List
  the list of the edges
"

%feature("docstring") gum::UndiGraph::empty
"
Check if the graph is empty.

Returns
-------
bool
    True if the graph is empty
"

%feature("docstring") gum::UndiGraph::emptyEdges
"
Check if the graph doesn't contains edges.

Returns
-------
bool
    True if the graph doesn't contains edges
"

%feature("docstring") gum::UndiGraph::eraseEdge
"
Erase the edge between n1 and n2.

Parameters
----------
n1 : int
  the id of the tail node
n2 : int
  the id of the head node
"

%feature("docstring") gum::UndiGraph::eraseNeighbours
"
Erase all the edges adjacent to a given node.

Parameters
----------
n : int
  the id of the node
"

%feature("docstring") gum::UndiGraph::eraseNode
"
Erase the node and all the adjacent edges.

Parameters
----------
id : int
  the id of the node
"

%feature("docstring") gum::UndiGraph::existsEdge
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

%feature("docstring") gum::UndiGraph::existsNode
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

%feature("docstring") gum::UndiGraph::hasUndirectedCycle
"
Checks whether the graph contains cycles.

Returns
-------
bool
    True if the graph contains a cycle
"

%feature("docstring") gum::UndiGraph::ids
"
.. note:: Deprecated in pyAgrum>0.13.0
    Please use nodes() instead
"

%feature("docstring") gum::UndiGraph::nodes
"
Returns
-------
set
    the set of ids
"


%feature("docstring") gum::UndiGraph::neighbours
"
Parameters
----------
id : int
    the id of the checked node

Returns
-------
Set
    The set of edges adjacent to the given node
"

%feature("docstring") gum::UndiGraph::partialUndiGraph
"
Parameters
----------
nodesSet : Set
    The set of nodes composing the partial graph

Returns
-------
pyAgrum.UndiGraph
    The partial graph formed by the nodes given in parameter
"

%feature("docstring") gum::UndiGraph::size
"
Returns
-------
int
    the number of nodes in the graph
"

%feature("docstring") gum::UndiGraph::sizeEdges
"
Returns
-------
int
    the number of edges in the graph
"

%feature("docstring") gum::UndiGraph::toDot
"
Returns
-------
str
    a friendly display of the graph in DOT format
"
