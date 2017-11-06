%feature("docstring") gum::IBayesNet::dim
"
Returns the dimension (the number of free parameters) in this BayesNet.

dim(G)=∑i∈nodes((ri−1)⋅qi) where ri is the number of instantiations of node i and qi is the number of instantiations of its parents. 
in IBayesNet

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
    the smallest value (not equal to 0) in the CPTs of the BayesNet
"

%feature("docstring") gum::IBayesNet::minParam
"
Returns
-------
double
    the smallest value in the CPTs of the BayesNet
"

%feature("docstring") gum::IBayesNet::toDot
"
Returns
-------
str
    a friendly display of the graph in DOT format 
"