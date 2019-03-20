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
	the Bayesian network used to find those modalities
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

%feature("docstring") gum::learning::genericBNLearner::useEM
"
Indicates if we use EM for parameter learning.

Parameters
----------
epsilon : double
	if epsilon=0.0 then EM is not used
	if epsilon>0 then EM is used and stops when the sum of the cumulative squared error on parameters is les than epsilon.
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


%feature("docstring") gum::learning::genericBNLearner::setSliceOrder
"
Set a partial order on the nodes.

Parameters
----------
l : list
        a list of sequences (composed of ids of rows or string)
"


%feature("docstring") gum::learning::BNLearner::useAprioriDirichlet
"
Use the Dirichlet apriori.

Parameters
----------
filename : str
        the Dirichlet related database
"

%feature("docstring") gum::learning::BNLearner::useAprioriSmoothing
"
Use the apriori smoothing.

Parameters
----------
weight : double
        pass in argument a weight if you wish to assign a weight to the smoothing, else the current weight of the learner will be used.
"

%feature("docstring") gum::learning::BNLearner::useGreedyHillClimbing
"
Indicate that we wish to use a greedy hill climbing algorithm.
"

%feature("docstring") gum::learning::genericBNLearner::useK2
"
Indicate that we wish to use K2.

Parameters
----------
order : list
        a list of ids
"

%feature("docstring") gum::learning::genericBNLearner::useLocalSearchWithTabuList
"
Indicate that we wish to use a local search with tabu list

Parameters
----------
tabu_size : int
        The size of the tabu list

nb_decrease : int
        The max number of changes decreasing the score consecutively that we allow to apply
"

%feature("docstring") gum::learning::genericBNLearner::hasMissingValues
"
Indicates whether there are missing values in the database.

Returns
-------
bool
    True if there are some missing values in the database.
"

%feature("docstring") gum::learning::BNLearner::useNoApriori
"
Use no apriori.
"

%feature("docstring") gum::learning::genericBNLearner::useAprioriBDeu
"
The BDeu apriori adds weight to all the cells of the counting tables.
In other words, it adds weight rows in the database with equally probable
values.

Parameters
----------
weight : double
	the apriori weight
"

%feature("docstring") gum::learning::BNLearner::useScoreAIC
"
Indicate that we wish to use an AIC score.
"

%feature("docstring") gum::learning::BNLearner::useScoreBD
"
Indicate that we wish to use a BD score.
"

%feature("docstring") gum::learning::BNLearner::useScoreBDeu
"
Indicate that we wish to use a BDeu score.
"

%feature("docstring") gum::learning::BNLearner::useScoreBIC
"
Indicate that we wish to use a BIC score.
"

%feature("docstring") gum::learning::BNLearner::useScoreK2
"
Indicate that we wish to use a K2 score.
"

%feature("docstring") gum::learning::BNLearner::useScoreLog2Likelihood
"
Indicate that we wish to use a Log2Likelihood score.
"


%feature("docstring") gum::learning::genericBNLearner::idFromName
"
Parameters
----------
var_names : str
	a variable's name

Returns
-------
int
	the column id corresponding to a variable name

Raises
------
gum.MissingVariableInDatabase
	If a variable of the BN is not found in the database.
"

%feature("docstring") gum::learning::genericBNLearner::learnDAG
"
learn a structure from a file

Returns
-------
pyAgrum.DAG
	the learned DAG
"


%feature("docstring") gum::learning::genericBNLearner::erasePossibleEdge
"
Allow the 2 arcs to be added if necessary.

Parameters
----------
arc : pyAgrum
	an arc
head :
	a variable's id (int)
tail :
	a variable's id (int)
head :
	a variable's name (str)
tail :
	a variable's name (str)
"
%feature("docstring") gum::learning::genericBNLearner::eraseForbiddenArc
"
Allow the arc to be added if necessary.

Parameters
----------
arc : pyAgrum
	an arc
head :
	a variable's id (int)
tail :
	a variable's id (int)
head :
	a variable's name (str)
tail :
	a variable's name (str)
"

%feature("docstring") gum::learning::genericBNLearner::eraseMandatoryArc
"
Parameters
----------
arc : pyAgrum
	an arc
head :
	a variable's id (int)
tail :
	a variable's id (int)
head :
	a variable's name (str)
tail :
	a variable's name (str)
"

%feature("docstring") gum::learning::genericBNLearner::addForbiddenArc
"
The arc in parameters won't be added.

Parameters
----------
arc : pyAgrum.Arc
	an arc
head :
	a variable's id (int)
tail :
	a variable's id (int)
head :
	a variable's name (str)
tail :
	a variable's name (str)
"

%feature("docstring") gum::learning::genericBNLearner::addMandatoryArc
"
Allow to add prior structural knowledge.

Parameters
----------
arc : pyAgrum.Arc
	an arc
head :
	a variable's id (int)
tail :
	a variable's id (int)
head :
	a variable's name (str)
tail :
	a variable's name (str)

Raises
------
gum.InvalidDirectedCycle
	If the added arc creates a directed cycle in the DAG
"


%feature("docstring") gum::learning::BNLearner::modalities
"
Returns
-------
vector<pos,size>
	the number of modalities of the database's variables.
"

%feature("docstring") gum::learning::genericBNLearner::nameFromId
"
Parameters
----------
id
	a node id

Returns
-------
str
	the variable's name
"

%feature("docstring") gum::learning::genericBNLearner::names
"
Returns
-------
List[str]
	the names of the variables in the database
"


%feature("docstring") gum::learning::BNLearner::setMaxIndegree
"
Parameters
----------
max_indegree : int
	the limit number of parents
"

%feature("docstring") gum::learning::genericBNLearner::setDatabaseWeight
"
Set the database weight.

Parameters
----------
weight : double
	the database weight
"

%feature("docstring") gum::learning::BNLearner::chi2
"
chi2 computes the chi2 statistic and pvalue for two columns, given a list of other columns.


Parameters
----------
name1: str
	the name of the first column

name2 : str
	the name of the second column

knowing : [str]
	the list of names of conditioning columns

Returns
-------
statistic,pvalue
	the chi2 statistic and the associated p-value as a Tuple
"

%feature("docstring") gum::learning::genericBNLearner::logLikelihood
"
logLikelihood computes the log-likelihood for the columns in vars, given the columns in the list knowing (optional)


Parameters
----------
vars: List[str]
	the name of the columns of interest

knowing : List[str]
	the (optional) list of names of conditioning columns

Returns
-------
double
	the log-likelihood (base 2)
"

%feature("docstring") gum::learning::genericBNLearner::nbRows
"
Return the number of row in the database


Returns
-------
int
	the number of rows in the database
"


%feature("docstring") gum::learning::genericBNLearner::nbCols
"
Return the nimber of columns in the database


Returns
-------
int
	the number of columns in the database
"

