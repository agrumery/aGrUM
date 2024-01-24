%feature("docstring") gum::DAGmodel
"
Abstract class used by IBayesNet and InfluenceDiagram.
"

%feature("docstring") gum::DAGmodel::log10DomainSize
"
Returns
-------
float
	The log10 domain size of the joint probability for the model.
"

%feature("docstring") gum::DAGmodel::completeInstantiation
"
Get an instantiation over all the variables of the model.

Returns
----------
  pyAgrum.instantiation
	the complete instantiation
"

%feature("docstring") gum::DAGmodel::dag
"
Returns
-------
pyAgrum.DAG
	a constant reference to the dag of this BayesNet.
"

%feature("docstring") gum::DAGmodel::empty
"
Returns
-------
bool
	True if the model is empty
"

%feature("docstring") gum::DAGmodel::hasSameStructure
"
Parameters
----------
pyAgrum.DAGmodel
	a direct acyclic model

Returns
-------
bool
    True if all the named node are the same and all the named arcs are the same
"

%feature("docstring") gum::DAGmodel::size
"
Returns
-------
int
    the number of nodes in the graph
"

%feature("docstring") gum::DAGmodel::sizeArcs
"
Returns
-------
int
    the number of arcs in the graph
"

%feature("docstring") gum::DAGmodel::topologicalOrder
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

%feature("docstring") gum::DAGmodel::moralGraph
"
Returns the moral graph of the BayesNet, formed by adding edges between all pairs of nodes that have a common child, and then making all edges in the graph undirected.

Returns
-------
pyAgrum.UndiGraph
	The moral graph
"

%feature("docstring") gum::DAGmodel::property
"
Warnings
--------
Unreferenced function
"

%feature("docstring") gum::DAGmodel::propertyWithDefault
"
Warnings
--------
Unreferenced function
"

%feature("docstring") gum::DAGmodel::setProperty
"
Warnings
--------
Unreferenced function
"

%feature("docstring") gum::DAGmodel::arcs
"
Returns
-------
list
	The lisf of arcs in the graph
"

%feature("docstring") gum::DAGmodel::toDot
"
Returns
-------
str
    a friendly display of the graph in DOT format
"

%feature("docstring") gum::DAGmodel::idFromName
"
Returns a variable's id given its name in the graph.

Parameters
----------
name : str
	The variable's name from which the id is returned.

Notes
-----
  A convenient shortcut for `g.variableFromName(name)` is `g[name]`.
  
Returns
-------
int :
	The variable's node id.

Raises
------
pyAgrum.NotFound
	If name does not match a variable in the graph
"

%feature("docstring") gum::DAGmodel::variableFromName
"
Parameters
----------
name : str
	a variable's name

Returns
-------
pyAgrum.DiscreteVariable
	the variable

Raises
------
pyAgrum.IndexError
	If the graph does not contain the variable
"


%feature("docstring") gum::DAGmodel::nodeId
"
Parameters
----------
var : pyAgrum.DiscreteVariable
	a variable

Returns
-------
int
	the id of the variable

Raises
------
pyAgrum.IndexError
	If the graph does not contain the variable
"

%feature("docstring") gum::DAGmodel::nodes
"
Returns
-------
set
    the set of ids
"

%feature("docstring") gum::DAGmodel::variable
"
Parameters
----------
id : int
	a variable's id
name : str
	a variable's name

Returns
-------
pyAgrum.DiscreteVariable
	the variable

Raises
------
pyAgrum.IndexError
	If the graph does not contain the variable
"

%feature("docstring") gum::DAGmodel::variableNodeMap
"
Returns
-------
pyAgrum.variableNodeMap
	the variable node map
"


%feature("docstring") gum::DAGmodel::ancestors
"
give the set of nodeid of ancestors of a node

Parameters
----------
norid : str|int
  the name or the id of the node

Returns
-------
Set[int]
  the set of ids of the ancestors of node `norid`.
"


%feature("docstring") gum::DAGmodel::descendants
"
give the set of nodeid of descendants of a node

Parameters
----------
norid : str|int
  the name or the id of the node

Returns
-------
Set[int]
  the set of ids of the descendants of node `norid`.
"

%feature("docstring") gum::DAGmodel::existsArc
"
Check if an arc exists

Parameters
---------
tail : str|int
  the name or id of the tail of the arc

head : str|int
  the name or the id of the head of the arc

Returns
-------
bool
  True if `tail->head` is an arc.
"

%feature("docstring") gum::DAGmodel::family
"
give the set of parents of a node and the node

Parameters
---------
norid : str|int
  the node

Returns
-------
Set[int]
  the set of nodeId of the family of the node `norid`
"

%feature("docstring") gum::DAGmodel::moralizedAncestralGraph
"
build a UndiGraph by moralizing the Ancestral Graph of a list of nodes

Parameters
----------
nodes : str|int|List[str|int]
  the list of of nodeIds or names

Warnings
--------
  gum.UndiGraph only knows NodeId. Hence the moralized ancestral graph does not include the names of the variables.graph

Returns
-------
gum.UndiGraph
  the moralized ancestral graph of the nodes
"

