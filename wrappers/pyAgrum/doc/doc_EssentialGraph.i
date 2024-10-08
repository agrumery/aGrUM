%feature("docstring") gum::EssentialGraph
"
Class building the essential graph from a BN.

Essential graph is a mixed graph (Chain Graph) that represents the class of markov equivalent Bayesian networks (with the same independency model).

EssentialGraph(m) -> EssentialGraph
    Parameters:
      - **m** (*pyAgrum.DAGmodel*) -- a DAGmodel
"

%feature("docstring") gum::EssentialGraph::arcs
"
Returns
-------
list
	The lisf of arcs in the EssentialGraph
"

%feature("docstring") gum::EssentialGraph::children
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

%feature("docstring") gum::EssentialGraph::cpt
"
Returns the CPT of a variable.

Parameters
----------
VarId : int
	A variable's id in the pyAgrum.EssentialGraph.
name : str
	A variable's name in the pyAgrum.EssentialGraph.

Returns
-------
pyAgrum.Potential
	The variable's CPT.

Raises
------
pyAgrum.NotFound
	If no variable's id matches varId.
"

%feature("docstring") gum::EssentialGraph::parents
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

%feature("docstring") gum::EssentialGraph::toDot
"
Returns
-------
str
    a friendly display of the graph in DOT format
"

%feature("docstring") gum::EssentialGraph::sizeEdges
"
Returns
-------
int
    the number of edges in the graph
"

%feature("docstring") gum::EssentialGraph::edges
"
Returns
-------
List
  the list of the edges
"

%feature("docstring") gum::EssentialGraph::neighbours
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

%feature("docstring") gum::EssentialGraph::sizeArcs
"
Returns
-------
int
    the number of arcs in the graph
"

%feature("docstring") gum::EssentialGraph::sizeNodes
"
Returns
-------
int
	the number of nodes in the graph
"

%feature("docstring") gum::EssentialGraph::size
"
Returns
-------
int
	the number of nodes in the graph
"

%feature("docstring") gum::EssentialGraph::pdag
"
Returns
-------
pyAgrum.PDAG
	the PDAG (Partially Directed Graph)
"

%feature("docstring") gum::EssentialGraph::idFromName
"
Parameters
----------
name : str
  the name of the variable in the model

Returns
-------
int
  the nodeId from the name of the variable in the model
"


%feature("docstring") gum::EssentialGraph::nameFromId
"
Parameters
----------
node : int
  the nodeId of the variable in the model

Returns
-------
str
  the name of the variable in the model from the nodeId
"
