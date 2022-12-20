%feature("docstring") gum::learning::BNLearner
"
BNLearner(filename,inducedTypes=True) -> BNLearner
    Parameters:
        * **source** (*str* or *pandas.DataFrame*) -- the data to learn from
        * **missingSymbols** (*List[str]*) -- list of string that will be interpreted as missing values (by default : ['?'])
        * **inducedTypes** (*Bool*) -- whether BNLearner should try to automatically find the type of each variable

BNLearner(filename,src) -> BNLearner
    Parameters:
        * **source** (*str* or *pandas.DataFrame) -- the data to learn from
        * **src** (*pyAgrum.BayesNet*) -- the Bayesian network used to find those modalities
        * **missingSymbols** (*List[str]*) -- list of string that will be interpreted as missing values (by default : ['?'])

BNLearner(learner) -> BNLearner
    Parameters:
        * **learner** (*pyAgrum.BNLearner*) -- the BNLearner to copy
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
	The dag passed in argument may have been learnt from a structure learning. In this case, if the score used to learn the structure has an implicit prior (like K2 which has a 1-smoothing prior), it is important to also take into account this implicit prior for parameter learning. By default, if a score exists, we will learn parameters by taking into account the prior specified by methods usePriorXXX () + the implicit prior of the score, else we just take into account the prior specified by usePriorXXX ()

Returns
-------
pyAgrum.BayesNet
	the learned BayesNet

Raises
------
pyAgrum.MissingVariableInDatabase
	If a variable of the BN is not found in the database
pyAgrum.UnknownLabelInDatabase
	If a label is found in the database that do not correspond to the variable
"

%feature("docstring") gum::learning::BNLearner::setInitialDAG
"
Parameters
----------
dag : pyAgrum.DAG
	an initial DAG structure
"

%feature("docstring") gum::learning::BNLearner::useEM
"
Indicates if we use EM for parameter learning.

Parameters
----------
epsilon : float
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

%feature("docstring") gum::learning::BNLearner::useNMLCorrection
"
Indicate that we wish to use the NML correction for 3off2 or MIIC
"

%feature("docstring") gum::learning::BNLearner::useMDLCorrection
"
Indicate that we wish to use the MDL correction for 3off2 or MIIC
"

%feature("docstring") gum::learning::BNLearner::useNoCorrection
"
Indicate that we wish to use the NoCorr correction for 3off2 or MIIC
"

%feature("docstring") gum::learning::IBNLearner::learnMixedStructure
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


%feature("docstring") gum::learning::BNLearner::setSliceOrder
"
Set a partial order on the nodes.

Parameters
----------
l : list
        a list of sequences (composed of ids of rows or string)
"


%feature("docstring") gum::learning::BNLearner::useDirichletPrior
"
Use the Dirichlet prior.

Parameters
----------
source : str|pyAgrum.BayesNet
        the Dirichlet related source (filename of a database or a Bayesian network)
weight : float (optional)
        the weight of the prior (the 'size' of the corresponding 'virtual database')
"

%feature("docstring") gum::learning::BNLearner::useSmoothingPrior
"
Use the prior smoothing.

Parameters
----------
weight : float
        pass in argument a weight if you wish to assign a weight to the smoothing, otherwise the current weight of the learner will be used.
"

%feature("docstring") gum::learning::BNLearner::useGreedyHillClimbing
"
Indicate that we wish to use a greedy hill climbing algorithm.
"

%feature("docstring") gum::learning::BNLearner::useK2
"
Indicate to use the K2 algorithm (which needs a total ordering of the variables).

Parameters
----------
order : list[int or str]
      sequences of (ids or name)
"

%feature("docstring") gum::learning::BNLearner::useLocalSearchWithTabuList
"
Indicate that we wish to use a local search with tabu list

Parameters
----------
tabu_size : int
        The size of the tabu list

nb_decrease : int
        The max number of changes decreasing the score consecutively that we allow to apply
"

%feature("docstring") gum::learning::IBNLearner::hasMissingValues
"
Indicates whether there are missing values in the database.

Returns
-------
bool
    True if there are some missing values in the database.
"

%feature("docstring") gum::learning::BNLearner::useNoPrior
"
Use no prior.
"

%feature("docstring") gum::learning::BNLearner::useBDeuPrior
"
The BDeu prior adds weight to all the cells of the counting tables.
In other words, it adds weight rows in the database with equally probable
values.

Parameters
----------
weight : float
	the prior weight
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


%feature("docstring") gum::learning::IBNLearner::idFromName
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
pyAgrum.MissingVariableInDatabase
	If a variable of the BN is not found in the database.
"

%feature("docstring") gum::learning::IBNLearner::learnDAG
"
learn a structure from a file

Returns
-------
pyAgrum.DAG
	the learned DAG
"


%feature("docstring") gum::learning::BNLearner::erasePossibleEdge
"
Allow the 2 arcs to be added if necessary.

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
%feature("docstring") gum::learning::BNLearner::eraseForbiddenArc
"
Allow the arc to be added if necessary.

Parameters
----------
arc: pyAgrum.Arc
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

%feature("docstring") gum::learning::BNLearner::eraseMandatoryArc
"
Parameters
----------
arc: pyAgrum.Arc
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

%feature("docstring") gum::learning::BNLearner::addForbiddenArc
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

%feature("docstring") gum::learning::BNLearner::setForbiddenArcs
"
assign a set of forbidden arcs

Parameters
----------
arcs: Set[Tuple[int|str,int|str]]
"

%feature("docstring") gum::learning::BNLearner::addMandatoryArc
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
pyAgrum.InvalidDirectedCycle
	If the added arc creates a directed cycle in the DAG
"


%feature("docstring") gum::learning::BNLearner::addPossibleEdge
"
assign a new possible edge

Warnings
--------
  By default, all edge is possible. However, once at least one possible edge is defined, all other edges not declared possible
  are considered as impossible.

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

%feature("docstring") gum::learning::IBNLearner::nameFromId
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

%feature("docstring") gum::learning::IBNLearner::names
"
Returns
-------
Tuple[str]
	the names of the variables in the database
"


%feature("docstring") gum::learning::BNLearner::setMaxIndegree
"
Parameters
----------
max_indegree : int
	the limit number of parents
"

%feature("docstring") gum::learning::IBNLearner::setDatabaseWeight
"
Set the database weight which is given as an equivalent sample size.

Warnings
--------
The same weight is assigned to all the rows of the learning database so that the sum of their
weights is equal to the value of the parameter `weight`.

Parameters
----------
weight : float
	the database weight
"

%feature("docstring") gum::learning::IBNLearner::setRecordWeight
"
Set the weight of the ith record

Parameters
----------
i : int
  the position  of the record in the database
weight : float
  the weight assigned to this record

Raises
------
pyAgrum.OutOfBounds
  if i is outside the set of indices of the records
"


%feature("docstring") gum::learning::IBNLearner::databaseWeight
"
Get the database weight which is given as an equivalent sample size.

Returns
-------
float
  The weight of the database
"

%feature("docstring") gum::learning::IBNLearner::recordWeight
"
Get the weight of the ith record

Parameters
----------
i : int
  the position  of the record in the database

Raises
------
pyAgrum.OutOfBounds
  if i is outside the set of indices of the records

Returns
-------
float
  The weight of the ith record of the database
"


%feature("docstring") gum::learning::BNLearner::pseudoCount
"
computes the pseudoCount (taking priors into account) of the list of variables


Parameters
----------
li: List[int|str]
	the list of variables

Returns
-------
pyAgrum.Potential
	the pseudo-count as a Potential
"

%feature("docstring") gum::learning::BNLearner::rawPseudoCount
"
computes the pseudoCount (taking priors into account) of the list of variables as a list of floats.


Parameters
----------
li: List[int|str]
	the list of variables

Returns
-------
List[float]
	the pseudo-count as a list of float
"

%feature("docstring") gum::learning::BNLearner::chi2
"
chi2 computes the chi2 statistic and p-value for two columns, given a list of other columns.


Parameters
----------
name1: str
	the name of the first column

name2 : str
	the name of the second column

knowing : List[str]
	the list of names of conditioning columns

Returns
-------
Tuple[float,float]
	the chi2 statistic and the associated p-value as a Tuple
"


%feature("docstring") gum::learning::BNLearner::G2
"
G2 computes the G2 statistic and p-value for two columns, given a list of other columns.


Parameters
----------
name1: str
	the name of the first column

name2 : str
	the name of the second column

knowing : List[str]
	the list of names of conditioning columns

Returns
-------
Tuple[float,float]
	the G2 statistic and the associated p-value as a Tuple
"

%feature("docstring") gum::learning::IBNLearner::mutualInformation
"
computes the mutual information between two columns, given a list of other columns (log2).

Warnings
--------
This function gives the 'raw' mutual information. If you want a version taking into account correction and prior, use
gum.BNLearner.correctedMutualInformation

Parameters
----------
name1: str
	the name of the first column

name2 : str
	the name of the second column

knowing : List[str]
	the list of names of conditioning columns

Returns
-------
Tuple[float,float]
	the G2 statistic and the associated p-value as a Tuple
"

%feature("docstring") gum::learning::IBNLearner::correctedMutualInformation
"
computes the mutual information between two columns, given a list of other columns (log2).

Warnings
--------
This function takes into account correction and prior. If you want the 'raw' mutual information, use
gum.BNLearner.mutualInformation


Parameters
----------
name1: str
	the name of the first column

name2 : str
	the name of the second column

knowing : List[str]
	the list of names of conditioning columns

Returns
-------
Tuple[float,float]
	the G2 statistic and the associated p-value as a Tuple
"

%feature("docstring") gum::learning::IBNLearner::score
"
Returns the value of the score currently in use by the BNLearner of a variable given a set of other variables

Parameters
----------
name1: str
	the name of the variable at the LHS of the conditioning bar

knowing : List[str]
	the list of names of the conditioning variables

Returns
-------
float
	the value of the score
"


%feature("docstring") gum::learning::IBNLearner::logLikelihood
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
float
	the log-likelihood (base 2)
"

%feature("docstring") gum::learning::BNLearner::nbRows
"
Return the number of row in the database


Returns
-------
int
	the number of rows in the database
"


%feature("docstring") gum::learning::BNLearner::nbCols
"
Return the number of columns in the database


Returns
-------
int
	the number of columns in the database
"

%feature("docstring") gum::learning::BNLearner::getNumberOfThreads
"
Return the number of threads used by the BNLearner during structure and parameter learning.

Returns
-------
int
	the number of threads used by the BNLearner during structure and parameter learning
"

%feature("docstring") gum::learning::BNLearner::setNumberOfThreads
"
If the parameter n passed in argument is different from 0, the BNLearner will use n threads during learning, hence overriding pyAgrum default number of threads.
If, on the contrary, n is equal to 0, the BNLearner will comply with pyAgrum default number of threads.

Parameters
----------
n : int
	the number of threads to be used by the BNLearner
"

%feature("docstring") gum::learning::BNLearner::isGumNumberOfThreadsOverriden
"
Indicates whether the BNLearner currently overrides pyAgrum default number of threads (see method setNumberOfThreads).

Returns
-------
bool
	A Boolean indicating whether the BNLearner currently overrides pyAgrum default number of threads
"
