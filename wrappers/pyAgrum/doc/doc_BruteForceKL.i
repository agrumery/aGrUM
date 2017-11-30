%feature("docstring") gum::BruteForceKL
"
Class representing an exact KL-divergence

Available constructors :
	
	``BruteForceKL(P,Q) -> BruteForceKL``

	``BruteForceKL(kl) -> BruteForceKL``

Parameters
----------
P : pyAgrum.IBayesNet
	a Bayesian network
Q : pyAgrum.IBayesNet
	anotre Bayesian network
kl : pyAgrum.KL
	the KL to copy

Warnings
--------
OperationNotAllowed raised if the 2BNs have not the same domain size of compatible node sets
"