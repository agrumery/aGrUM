%feature("docstring") gum::CliqueGraph
"
CliqueGraph represents a Clique Graph.

Available constructors:
    ``CliqueGraph() -> CliqueGraph``

    ``CliqueGraph(src) -> CliqueGraph``

    ``CliqueGraph(nodes_size) -> CliqueGraph``

    ``CliqueGraph(nodes_size, nodes_resize_policy) -> CliqueGraph``

    ``CliqueGraph(nodes_size, nodes_resize_policy, edges_size) -> CliqueGraph``

    ``CliqueGraph(nodes_size, nodes_resize_policy, edges_size, edges_size_policy) -> CliqueGraph``

Parameters
----------
src : pyAgrum.CliqueGraph
  the CliqueGraph to copy
nodes_size : int
  the number of nodes
nodes_resize_policy : bool
edges_size : int
  the number of edges
edges_size_policy : bool
"

%feature("docstring") gum::CliqueGraph::addToClique
"
Change the set of nodes included into a given clique and returns the new set 

Parameters
----------
clique_id : int
  the id of the clique
node_id : int
  the id of the node

Warnings
--------
NotFound raised if clique_id does not exist

Warnings
--------
DuplicateElement raised if clique_id set already contains the ndoe
"

%feature("docstring") gum::CliqueGraph::clearEdges
"
Remove all edges and their separators 
"

%feature("docstring") gum::CliqueGraph::clique
"
Parameters
----------
idClique : int
  the id of the clique

Returns
-------
Set
  The set of nodes included in the clique

Warnings
--------
NotFound raised if the clique does not belong to the clique graph
"

%feature("docstring") gum::CliqueGraph::container
"
Parameters
----------
idNode : int
  the id of the node

Returns
-------
int
  the id of a clique containing the node

Warnings
--------
NotFound raised if no clique contains idNode  
"

%feature("docstring") gum::CliqueGraph::containerPath
"
Parameters
----------
node1 : int
  the id of one node
node2 : int
  the id of the other node

Returns
-------
List
  a path from a clique containing node1 to a clique containing node2

Warnings
--------
NotFound raised if such path cannot be found
"

%feature("docstring") gum::CliqueGraph::eraseFromClique
"
Remove a node from a clique

Parameters
----------
clique_id : int
  the id of the clique
node_id : int
  the id of the node

Warnings
--------
NotFound raised if clique_id does not exist
"

%feature("docstring") gum::CliqueGraph::hasRunningIntersection
"
Returns
-------
bool
  True if the running intersection property holds 
"

%feature("docstring") gum::CliqueGraph::isJoinTree
"
Returns
-------
bool
  True if the graph is a join tree 
"

%feature("docstring") gum::CliqueGraph::separator
"
Parameters
----------
edge : pyAgrum.Edge
  the edge to be checked
clique1 : int 
  one extremity of the edge
clique : int 
  the other extremity of the edge

Returns
-------
Set
  the separator included in a given edge

Warnings
--------
NotFound raised if the edge does not belong to the clique graph
"

%feature("docstring") gum::CliqueGraph::setClique
"
changes the set of nodes included into a given clique

Parameters
----------
idClique : int
  the id of the clique
new_clique : Set
  the new set of nodes to be included in the clique

Warnings
--------
NotFound exception if idClique is not a clique of the graph
"