%feature("docstring") gum::CliqueGraph
"
CliqueGraph represents a Clique Graph.

CliqueGraph() -> CliqueGraph
    default constructor

CliqueGraph(src) -> CliqueGraph
    Parameter
        * **src** (*pyAgrum.CliqueGraph*) -- the CliqueGraph to copy
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

Raises
------
gum.NotFound
  If clique_id does not exist
gum.DuplicateElement
  If clique_id set already contains the ndoe
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

Raises
------
gum.NotFound
  If the clique does not belong to the clique graph
"

%feature("docstring") gum::CliqueGraph::separator
"
        Parameters
        ----------
        clique1 : int 
          one extremity of the edge
        clique : int 
          the other extremity of the edge

        Returns
        -------
        Set
          the separator included in a given edge

        Raises
        ------
        gum.NotFound
          If the edge does not belong to the clique graph
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

Raises
------
gum.NotFound
  If no clique contains idNode  
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

Raises
------
gum.NotFound 
  If such path cannot be found
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

Raises
------
gum.NotFound
  If clique_id does not exist
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

Raises
------
gum.NotFound
  If the edge does not belong to the clique graph
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

Raises
------
gum.NotFound
  If idClique is not a clique of the graph
"