%feature("docstring") gum::IApproximationSchemeConfiguration::addForbiddenArc
"
The arc in parameters won't be added.

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

%feature("docstring") gum::IApproximationSchemeConfiguration::addMandatoryArc
"
Allow to add prior structural knowledge.

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

Warnings
--------
InvalidDirectedCycle raised if the added arc creates a directed cycle in the DAG
"

%feature("docstring") gum::IApproximationSchemeConfiguration::burnIn
"
Returns
-------
int
	size of burn in on number of iteration
"

%feature("docstring") gum::IApproximationSchemeConfiguration::currentTime
"
Returns
-------
double
	get the current running time in second (double)
"

%feature("docstring") gum::IApproximationSchemeConfiguration::epsilon
"
Returns
-------
double
	the value of epsilon
"

%feature("docstring") gum::IApproximationSchemeConfiguration::eraseForbiddenArc
"
Allow the arc in parameter to be added if necessary.

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

%feature("docstring") gum::IApproximationSchemeConfiguration::eraseMandatoryArc
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

%feature("docstring") gum::IApproximationSchemeConfiguration::history
"
Returns
-------
tuple
	the scheme history

Warnings
--------
OperationNotAllowed	Raised if the scheme did not performed or if verbosity is set to false
"

%feature("docstring") gum::IApproximationSchemeConfiguration::idFromName
"
Parameters
----------
var_names : str
	a variable's name

Returns
-------
int
	the node id corresponding to a variable name

Warnings
--------
MissingVariableInDatabase raised if a variable of the BN is not found in the database.
"

%feature("docstring") gum::IApproximationSchemeConfiguration::learnDAG
"
learn a structure from a file (must have read the db before)

Returns
-------
pyAgrum.DAG
	the learned DAG
"

%feature("docstring") gum::IApproximationSchemeConfiguration::maxIter
"
Returns
-------
int
	the criterion on number of iterations
"

%feature("docstring") gum::IApproximationSchemeConfiguration::maxTime
"
Returns
-------
double
	the timeout(in seconds)
"

%feature("docstring") gum::IApproximationSchemeConfiguration::messageApproximationScheme
"
Returns
-------
str
	the approximation scheme message
"

%feature("docstring") gum::IApproximationSchemeConfiguration::minEpsilonRate
"
Returns
-------
double
	the value of the minimal epsilon rate
"

%feature("docstring") gum::IApproximationSchemeConfiguration::modalities
"
Returns
-------
vector<pos,size>
	the number of modalities of the database's variables.
"

%feature("docstring") gum::IApproximationSchemeConfiguration::nameFromId
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

%feature("docstring") gum::IApproximationSchemeConfiguration::names
"
Returns
-------
str
	the names of the variables in the database
"

%feature("docstring") gum::IApproximationSchemeConfiguration::nbrIterations
"
Returns
-------
int
	the number of iterations
"

%feature("docstring") gum::IApproximationSchemeConfiguration::periodSize
"
Returns
-------
int
	the number of samples between 2 stopping

Warnings
--------
OutOfLowerBound raised if p<1
"

%feature("docstring") gum::IApproximationSchemeConfiguration::setAprioriWeight
"
Parameters
----------
weight : double
	the apriori weight
"

%feature("docstring") gum::IApproximationSchemeConfiguration::setBurnIn
"
Parameters
----------
b : int
	size of burn in on number of iteration
"

%feature("docstring") gum::IApproximationSchemeConfiguration::setEpsilon
"
Parameters
----------
eps : double
	the epsilon we want to use

Warnings
--------
OutOfLowerBound if eps<0
"

%feature("docstring") gum::IApproximationSchemeConfiguration::setMaxIndegree
"
Parameters
----------
max_indegree : int
	the limit number of parents
"

%feature("docstring") gum::IApproximationSchemeConfiguration::setMaxIter
"
Parameters
----------
max : int
	the maximum number of iteration

Warnings
--------
OutOfLowerBound raised if max <= 1
"

%feature("docstring") gum::IApproximationSchemeConfiguration::setMaxTime
"
Parameters
----------
tiemout : double
	stopping criterion on timeout (in seconds)

Warnings
--------
OutOfLowerBound raised if timeout<=0.0
"

%feature("docstring") gum::IApproximationSchemeConfiguration::setMinEpsilonRate
"
Parameters
----------
rate : double
	the minimal epsilon rate
"

%feature("docstring") gum::IApproximationSchemeConfiguration::setPeriodSize
"
Parameters
----------
p : int
	number of samples between 2 stopping

Warnings
--------
OutOfLowerBound raised if p<1
"

%feature("docstring") gum::IApproximationSchemeConfiguration::setSliceOrder
"
Warnings
--------
A Faire
"

%feature("docstring") gum::IApproximationSchemeConfiguration::setVerbosity
"
Parameters
----------
v : bool
	verbosity
"

%feature("docstring") gum::IApproximationSchemeConfiguration::useAprioriDirichlet
"
Use the Dirichlet apriori.

Parameters
----------
filename : str
	the Dirichlet related database
"

%feature("docstring") gum::IApproximationSchemeConfiguration::useAprioriSmoothing
"
Use the apriori smoothing.

Parameters
----------
weight : double
	pass in argument a weight if you wish to assign a weight to the smoothing, else the current weight of the learner will be used.
"

%feature("docstring") gum::IApproximationSchemeConfiguration::useGreedyHillClimbing
"
Indicate that we wish to use a greedy hill climbing algorithm.
"

%feature("docstring") gum::IApproximationSchemeConfiguration::useK2
"
Indicate that we wish to use K2.

Parameters
----------
order : list
	a list of ids
"

%feature("docstring") gum::IApproximationSchemeConfiguration::useLocalSearchWithTabuList
"
Indicate that we wish to use a local search with tabu list

Parameters
----------
tabu_size : int
	The size of the tabu list
nb_decrease : int
	The max number of changes decreasing the score consecutively that we allow to apply
"

%feature("docstring") gum::IApproximationSchemeConfiguration::useNoApriori
"
Use no apriori.
"

%feature("docstring") gum::IApproximationSchemeConfiguration::useScoreAIC
"
Indicate that we wish to use an AIC score.
"

%feature("docstring") gum::IApproximationSchemeConfiguration::useScoreBD
"
Indicate that we wish to use a BD score.
"

%feature("docstring") gum::IApproximationSchemeConfiguration::useScoreBDeu
"
Indicate that we wish to use a BDeu score.
"

%feature("docstring") gum::IApproximationSchemeConfiguration::useScoreBIC
"
Indicate that we wish to use a BIC score.
"

%feature("docstring") gum::IApproximationSchemeConfiguration::useScoreK2
"
Indicate that we wish to use a K2 score.
"

%feature("docstring") gum::IApproximationSchemeConfiguration::useScoreLog2Likelihood
"
Indicate that we wish to use a Log2Likelihood score.
"

%feature("docstring") gum::IApproximationSchemeConfiguration::verbosity
"
Returns
-------
bool
	True if the verbosity is enabled
"
