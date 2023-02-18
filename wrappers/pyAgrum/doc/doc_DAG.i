// docs from parent gum.DiGraph are automatically added
%feature("docstring") gum::DAG
"
DAG represents a Directed Graph.

DAG() -> DAG
    default constructor

DAG(src) -> DAG
    Parameters:
        - **src** (*pyAgrum.DAG*) -- the digraph to copy
"

%feature("docstring") gum::DAG::addArc
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
    if the arc would create a cycle.
"

%feature("docstring") gum::DAG::topologicalOrder
"
Returns
-------
List
    the list of the nodes Ids in a topological order

Raises
------
pyAgrum.InvalidDirectedCycle
  If this graph contains cycles
"

%feature("docstring") gum::DAG::hasDirectedPath
"
Check if a directed path exists bewteen from and to.

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

%feature("docstring") gum::DAG::moralGraph
"
Creates the `gum.UndiGraph` corresponding to the moralization of the DAG

Returns
-------
gum.UndiGraph
    the moral graph
"

%feature("docstring") gum::DAG::dSeparation
"
Check if X and Y are dSeparated by Z.

Parameters:
-----------
X : int|Sequence[int]
  a node set or a node
Y : int|Sequence[int]
  a node set or a node
Z : int|Sequence[int]
  a node set or a node

Returns
-------
bool
    true if X and Y are d-separated by Z.
"


