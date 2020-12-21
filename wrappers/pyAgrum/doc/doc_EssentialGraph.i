%feature("dosctring") gum::EssentialGraph
"
Class building the essential graph from a BN.

Essential graph is a mixed graph (Chain Graph) that represents the class of markov equivalent Bayesian networks (with the same independency model).

EssentialGraph(m) -> EssentialGraph
    Parameters:
      * **m** (*pyAgrum.DAGmodel*) -- a DAGmodel
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
gum.NotFound
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

%feature("docstring") gum::EssentialGraph::mixedGraph
"
Returns
-------
pyAgrum.MixedGraph
	the mixed graph
"