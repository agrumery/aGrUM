%feature("docstring") gum::IBayesNet
"
Abstract class used by BayesNet.
"

%feature("docstring") gum::IBayesNet::dim
"
Returns the dimension (the number of free parameters) in this BayesNet.

Returns
-------
int
	the dimension of the BayesNet
"

%feature("docstring") gum::IBayesNet::jointProbability
"
Parameters
----------
i : pyAgrum.instantiation
	an instantiation of the variables

Returns
-------
double
	a parameter of the joint probability for the BayesNet

Warnings
--------
a variable not present in the instantiation is assumed to be instantiated to 0
"

%feature("docstring") gum::IBayesNet::log2JointProbability
"
Parameters
----------
i : pyAgrum.instantiation
	an instantiation of the variables

Returns
-------
double
	a parameter of the log joint probability for the BayesNet

Warnings
--------
a variable not present in the instantiation is assumed to be instantiated to 0
"

%feature("docstring") gum::IBayesNet::maxNonOneParam
"
Returns
-------
double
	The biggest value (not equal to 1) in the CPTs of the BayesNet
"

%feature("docstring") gum::IBayesNet::maxParam
"
Returns
-------
double
    the biggest value in the CPTs of the BayesNet
"

%feature("docstring") gum::IBayesNet::maxVarDomainSize
"
Returns
-------
int
	the biggest domain size among the variables of the BayesNet
"

%feature("docstring") gum::IBayesNet::minNonZeroParam
"
Returns
-------
double
    the smallest value (not equal to 0) in the CPTs of the IBayesNet
"

%feature("docstring") gum::IBayesNet::minParam
"
Returns
-------
double
    the smallest value in the CPTs of the IBayesNet
"

%feature("docstring") gum::IBayesNet::toDot
"
Returns
-------
str
    a friendly display of the graph in DOT format
"

%feature("docstring") gum::IBayesNet::arcs
"
Returns
-------
list
	The lisf of arcs in the IBayesNet
"

%feature("docstring") gum::IBayesNet::children
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

%feature("docstring") gum::IBayesNet::cpt
"
Returns the CPT of a variable.

Parameters
----------
VarId : int
	A variable's id in the pyAgrum.IBayesNet.
name : str
	A variable's name in the pyAgrum.IBayesNet.

Returns
-------
pyAgrum.Potential
	The variable's CPT. 

Raises
------
gum.NotFound 
	If no variable's id matches varId. 
"

%feature("docstring") gum::IBayesNet::parents
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

%feature("docstring") gum::IBayesNet::ids
"
Returns
-------
list :
	The list variables ids.
"

%feature("docstring") gum::IBayesNet::names
"
Returns
-------
list
	The names of the graph variables
"

%feature("docstring") gum::IBayesNet::minimalCondSet
"
Returns, given one or many targets and a list of variables, the minimal set of those needed to calculate the target/targets.

Parameters
----------
target : int
	The id of the target
targets : list
	The ids of the targets
list : list
	The list of available variables

Returns
-------
Set
	The minimal set of variables
"