%feature("docstring") BNGenerator
"
BNGenerator is used to easily generate Bayesian networks.

BNGenerator() -> BNGenerator
    default constructor
"

%feature("docstring") BNGenerator::generate
"
Generate a new Bayesian network 

Parameters
----------
n_nodes : int
	the number of nodes (default=10)
n_arcs : int
	the number of arcs (default=15)
n_nodmax : int
	the max number of modalities for a node (default=4)

Returns
-------
pyAgrum.BayesNet
	the generated Bayesian network

Raises
------
gum.OperationNotAllowed
	If n_modmax < 2
gum.OperationNotAllowed
	If n_arcs is incompatible with n_nodes (not enough arcs)
"