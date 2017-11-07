%feature("docstring") gum::learning::BNLearner
"

"

%feature("docstring") gum::learning::BNLearner::asIApproximationSchemeConfiguration
"
Warnings
--------
A Faire
"

%feature("docstring") gum::learning::BNLearner::learnBN
"
learn a BayesNet from a file (must have read the db before) 

Returns
-------
pyAgrum.BayesNet
	the learned BayesNet
"

%feature("docstring") gum::learning::BNLearner::learnParameters
"
learns a BN (its parameters) when its structure is known.

Parameters
----------
dag : pyAgrum.DAG
bn : pyAgrum.BayesNet
take_into_account_score : bool
	The dag passed in argument may have been learnt from a structure learning. In this case, if the score used to learn the structure has an implicit apriori (like K2 which has a 1-smoothing apriori), it is important to also take into account this implicit apriori for parameter learning. By default, if a score exists, we will learn parameters by taking into account the apriori specified by methods useAprioriXXX () + the implicit apriori of the score, else we just take into account the apriori specified by useAprioriXXX () 

Returns
-------
pyAgrum.BayesNet
	the learned BayesNet

Warnings
--------
MissingVariableInDatabase if a variable of the BN is not found in the database

Warnings
--------
UnknownLabelInDatabase raise if a label is found in the database that do not correspond to the variable
"

%feature("docstring") gum::learning::BNLearner::setInitialDAG
"
Parameters
----------
dag : pyAgrum.DAG
	an initial DAG structure
"