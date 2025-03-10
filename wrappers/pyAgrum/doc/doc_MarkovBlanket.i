%feature("docstring") gum::MarkovBlanket
"
Class building the Markov blanket of a node in a graph.

MarkovBlanket(m,n) -> MarkovBlanket
    Parameters:
        - **m** (*pyAgrum.DAGmodel*) -- a DAGmodel
        - **n** (int) -- a node id

MarkovBlanket(m,name) -> MarkovBlanket
    Parameters:
        - **m** (*pyAgrum.DAGmodel*) -- a DAGmodel
        - **name** (*str*) -- a node name
"

%feature("docstring") gum::MarkovBlanket::dag
"
Returns
-------
pyAgrum.DAG
  a copy of the DAG
"

%feature("docstring") gum::MarkovBlanket::toDot
"
Returns
-------
str
    a friendly display of the graph in DOT format
"

%feature("docstring") gum::MarkovBlanket::parents
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

%feature("docstring") gum::MarkovBlanket::children
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

%feature("docstring") gum::MarkovBlanket::sizeArcs
"
Returns
-------
int
    the number of arcs in the graph
"

%feature("docstring") gum::MarkovBlanket::arcs
"
Returns
-------
List
	the list of the arcs
"

%feature("docstring") gum::MarkovBlanket::nodes
"
Returns
-------
set
    the set of ids
"

%feature("docstring") gum::MarkovBlanket::size
"
Returns
-------
int
    the number of nodes in the graph
"

%feature("docstring") gum::MarkovBlanket::sizeNodes
"
Returns
-------
int
	the number of nodes in the graph
"

%feature("docstring") gum::MarkovBlanket::hasSameStructure
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
