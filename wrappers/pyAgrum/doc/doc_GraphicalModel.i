%feature("docstring") gum::GraphicalModel
"
Abstract class for all PGM (associating set of variables and a graph).
"

%feature("docstring") gum::GraphicalModel::names
"
Set of names of variables in the model

Returns
-------
Set[str]
	The names of the graph variables
"

%feature("docstring") gum::GraphicalModel::ids
"
Set of ids for a list of names of variables in the model

Parameters
----------
lov : List[str]
  List of variable names

Returns
-------
List[int]
	The ids for the list of names of the graph variables
"
