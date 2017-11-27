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

Warnings
--------
	NotFound raised if no variable's id matches varId. 
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