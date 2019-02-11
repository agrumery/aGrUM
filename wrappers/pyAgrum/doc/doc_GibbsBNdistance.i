%feature("docstring") gum::GibbsBNdistance
"
Class representing a Gibbs-Approximated computation of divergence and distance between BNs

Available constructors :

	``GibbsBNdistance(P,Q) -> GibbsBNdistance``

	``GibbsBNdistance(gbnd) -> GibbsBNdistance``

Parameters
----------
P : pyAgrum.IBayesNet
	a Bayesian network
Q : pyAgrum.IBayesNet
	anotre Bayesian network
gbnd : pyAgrum.GibbsBNdistance
	the Gibbs BNdistance to copy

Raises
------
gum.OperationNotAllowed
	If the 2BNs have not the same domain size of compatible node sets
"

%feature("docstring") gum::GibbsBNdistance::nbrDrawnVar
"
Returns
-------
int
  the number of variable drawn at each iteration
"

%feature("docstring") gum::GibbsBNdistance::isDrawnAtRandom
"
Returns
-------
bool
  True if variables are drawn at random
"

%feature("docstring") gum::GibbsBNdistance::setDrawnAtRandom
"
Parameters
----------
_atRandom : bool
  indicates if variables should be drawn at random
"

%feature("docstring") gum::GibbsBNdistance::setNbrDrawnVar
"
Parameters
----------
_nbr : int
  the number of variables to be drawn at each iteration
"

%feature("docstring") gum::GibbsBNdistance::asIApproximationSchemeConfiguration
"
Returns
-------
asIApproximationSchemeConfiguration
"
