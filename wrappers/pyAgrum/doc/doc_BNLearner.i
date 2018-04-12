%feature("docstring") gum::learning::BNLearner
"
Available constructors:
	``BNLearner(filename) -> BNLearner``

	``BNLearner(filename,src,parse_database=false) -> BNLearner``

	``BNLearner(learner) -> BNLearner``

Parameters
----------
filename: str
	the file to learn from
src : pyAgrum.BayesNet
	the Bayesian network used to find thoes modalities
parse_database : bool
	if true, the modalities specified by the user will be considered as a superset of the modalities of the variables.
learner : pyAgrum.BNLearner
	the BNLearner to copy
"

%feature("docstring") gum::learning::BNLearner::asIApproximationSchemeConfiguration
"
Returns
-------
asIApproximationSchemeConfiguration
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

Raises
------
gum.MissingVariableInDatabase
	If a variable of the BN is not found in the database
gum.UnknownLabelInDatabase
	If a label is found in the database that do not correspond to the variable
"

%feature("docstring") gum::learning::BNLearner::setInitialDAG
"
Parameters
----------
dag : pyAgrum.DAG
	an initial DAG structure
"

%feature("docstring") gum::learning::BNLearner::useMIIC
"
Indicate that we wish to use MIIC.
"

%feature("docstring") gum::learning::BNLearner::use3off2
"
Indicate that we wish to use 3off2.
"

%feature("docstring") gum::learning::BNLearner::useNML
"
Indicate that we wish to use the NML correction for 3off2 or MIIC
"

%feature("docstring") gum::learning::BNLearner::useMDL
"
Indicate that we wish to use the MDL correction for 3off2 or MIIC
"

%feature("docstring") gum::learning::BNLearner::useNoCorr
"
Indicate that we wish to use the NoCorr correction for 3off2 or MIIC
"

%feature("docstring") gum::learning::BNLearner::learnMixedStructure
"
Warnings
--------
learner must be using 3off2 or MIIC algorithm

Returns
-------
pyAgrum.EssentialGraph
	the learned structure as an EssentialGraph
"

%feature("docstring") gum::learning::BNLearner::latentVariables
"
Warnings
--------
learner must be using 3off2 or MIIC algorithm

Returns
-------
list
	the list of latent variables
"
