%feature("docstring") gum::GibbsKL
"
Class representing a Gibbs-Approximated KL-divergence 

Available constructors :
	
	``GibbsKL(P,Q) -> GibbsKL``

	``GibbsKL(kl) -> GibbsKL``

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

%feature("docstring") gum::GibbsKL::nbrDrawnVar
"
Returns
-------
int
  the number of variable drawn at each iteration
"

%feature("docstring") gum::GibbsKL::isDrawnAtRandom
"
Returns
-------
bool
  True if variables are drawn at random
"

%feature("docstring") gum::GibbsKL::setDrawnAtRandom
"
Parameters
----------
_atRandom : bool
  indicates if variables should be drawn at random
"

%feature("docstring") gum::GibbsKL::setNbrDrawnVar
"
Parameters
----------
_nbr : int
  the number of variables to be drawn at each iteration
"

%feature("docstring") gum::GibbsKL::asIApproximationSchemeConfiguration
"
Returns
-------
asIApproximationSchemeConfiguration
"