%feature("docstring") gum::ExactBNdistance
"
Class representing exacte computation of divergence and distance between BNs

Available constructors :

	``ExactBNdistance(P,Q) -> ExactBNdistance``

	``ExactBNdistance(ebnd) -> ExactBNdistance``

Parameters
----------
P : pyAgrum.IBayesNet
	a Bayesian network
Q : pyAgrum.IBayesNet
	anotre Bayesian network
ebnd : pyAgrum.ExactBNdistance
	the exact BNdistance to copy

Raises
------
gum.OperationNotAllowed
	If the 2BNs have not the same domain size of compatible node sets
"
