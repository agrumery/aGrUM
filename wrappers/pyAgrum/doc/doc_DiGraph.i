%feature("docstring") gum::DiGraph
"
DiGraph represents a Directed Graph.

Available constructors:
    ``DiGraph() -> DiGraph``

    ``DiGraph(src) -> DiGraph``

Parameters
----------
src : :class: DiGraph
  the digraph to copy

"

%feature("docstring") gum::DiGraph::addArc
"
Add an arc from tail to head.

Parameters
----------
tail : int
  the id of the tail node
head : int
  the id of the head node

Warnings
--------
InvalidNode raised if head or tail does not belong to the graph nodes.
"

%feature("docstring") gum::DiGraph::addNode
"
Returns
-------
int
  the new NodeId
"

%feature("docstring") gum::DiGraph::addNodeWithId
"
Add a node by choosing a new NodeId.

Returns
-------
int
  the new NodeId
"

%feature("docstring") gum::DiGraph::arcs
"
Returns
-------
List
	the list of the arcs
"

%feature("docstring") gum::DiGraph::children
"
Parameters
----------
id : int
  the id of the parent

Returns
-------
Set
	the set of all the children
"

%feature("docstring") gum::DiGraph::clear
"
Remove all the nodes and arcs from the graph.
"

%feature("docstring") gum::DiGraph::empty
"
Check if the graph is empty.

Returns
-------
bool
    True if the graph is empty
"

%feature("docstring") gum::DiGraph::emptyArcs
"
Check if the graph doesn't contains arcs.

Returns
-------
bool
    True if the graph doesn't contains arcs
"

%feature("docstring") gum::DiGraph::eraseArc
"
Erase the arc between n1 and n2.

Parameters
----------
n1 : int
	the id of the tail node
n2 : int
	the id of the head node
"
%feature("docstring") gum::DiGraph::eraseChildren
"
Erase the arcs heading through the node's children.

Parameters
----------
n : int
	the id of the parent node
"

%feature("docstring") gum::DiGraph::eraseNode
"
Erase the node and all the related arcs.

Parameters
----------
id : int
	the id of the node
"

%feature("docstring") gum::DiGraph::eraseParents
"
Erase the arcs coming to the node.

Parameters
----------
n : int
	the id of the child node
"

%feature("docstring") gum::DiGraph::existsArc
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

%feature("docstring") gum::DiGraph::existsNode
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

%feature("docstring") gum::DiGraph::ids
"
Returns
-------
List
    the list of ids
"

%feature("docstring") gum::DiGraph::parents
"
Parameters
----------
id :
	The id of the child node

Returns
-------
Set
    the set of the parents ids.
"

%feature("docstring") gum::DiGraph::size
"
Returns
-------
int
    the number of nodes in the graph
"

%feature("docstring") gum::DiGraph::sizeArcs
"
Returns
-------
int
    the number of arcs in the graph
"

%feature("docstring") gum::DiGraph::toDot
"
Returns
-------
str
    a friendly display of the graph in DOT format 

Examples
--------
>>> import pyAgrum as gum
>>> bn = gum.DiGraph()
>>> a = bn.addNode()
>>> b = bn.addNode()
>>> bn.addArc(a,b)
>>> bn.toDot()
digraph {
	0;
	1;
	0 -> 1;
}
"

%feature("docstring") gum::DiGraph::topologicalOrder
"
Returns
-------
List
    the list of the nodes Ids in a topological order

Warnings
--------
InvalidDirectedCycle raised if this graph contains cycles
"