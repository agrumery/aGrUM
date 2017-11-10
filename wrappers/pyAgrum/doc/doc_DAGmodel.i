%feature("docstring") gum::DAGmodel::log10DomainSize
"
Returns
-------
double
	The log10 domain size of the joint probability for the model.
"

%feature("docstring") gum::DAGmodel::completeInstantiation
"
Get an instantiation over all the variables of the model.

Parameters
----------
i : pyAgrum.instantiation
	the instantiation to complete
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

Warnings
--------
InvalidDirectedCycle raised if this graph contains cycles
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