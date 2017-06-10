%feature("docstring") gum::DiGraph
"
DiGraph represents a Directed Graph.

Available constructors:
    `DiGraph() -> DiGraph`

    `DiGraph(src) -> DiGraph`

Parameters
----------
src : :class: DiGraph
  the digraph to copy

"

%feature("docstring") gum::DiGraph::addNode
"
Add a node by choosing a new NodeId

Returns
-------
int
  the new NodeId
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

