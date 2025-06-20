/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/




%feature("docstring") gum::learning::BNLearner
"
This class provides functionality for learning Bayesian Networks from data.

BNLearner(filename,inducedTypes=True) -> BNLearner
    Parameters:
        - **source** (*str* or *pandas.DataFrame*) -- the data to learn from
        - **missingSymbols** (*List[str]*) -- list of strings that will be interpreted as missing values (by default : `?`)
        - **inducedTypes** (*Bool*) -- whether BNLearner should try to automatically find the type of each variable

BNLearner(filename,src) -> BNLearner
    Parameters:
        - **source** (*str* or *pandas.DataFrame*) -- the data to learn from
        - **src** (*pyagrum.BayesNet*) -- the Bayesian network used to find those modalities
        - **missingSymbols** (*List[str]*) -- list of strings that will be interpreted as missing values (by default : `?`)

BNLearner(learner) -> BNLearner
    Parameters:
        - **learner** (*pyagrum.BNLearner*) -- the BNLearner to copy
"

%feature("docstring") gum::learning::IBNLearner::learnPDAG
"
learn a PDAG from a file

Warnings
--------
  The learning method must be constraint-based (MIIC, etc.) and not score-based (K2, GreedyHillClimbing, etc.)

Returns
-------
pyagrum.PDAG
	the learned PDAG
"

%feature("docstring") gum::learning::BNLearner::learnBN
"
learn a BayesNet from a file (must have read the db before)

Returns
-------
pyagrum.BayesNet
	the learned BayesNet
"


%feature("docstring") gum::learning::BNLearner::state
"
Returns a dictionary containing the current state of the BNLearner.

Returns
-------
Dict[str,Any]
    a dictionary containing the current state of the BNLearner.
"


%feature("docstring") gum::learning::BNLearner::learnParameters
"
Create a new BN copying its structure from the argument (dag or BN) and learning its parameters
from the database w.r.t the BNLearner's state (priors, etc.).

Warnings
--------
When using a `pyagrum.DAG` as input parameter, NodeIds in the dag and index of rows in the database must fit in order to coherently fix the structure of the BN.
Generally, it is safer to use a `pyagrum.BayesianNet` as input or even to use `pyagrum.BNLearner.fitParameters`.

Parameters
----------
dag : pyagrum.DAG
bn : pyagrum.BayesNet
take_into_account_score : bool
	The dag passed in argument may have been learnt from a structure learning. In this case, if the score used to learn the structure has an implicit prior (like K2 which has a 1-smoothing prior), it is important to also take into account this implicit prior for parameter learning. By default (`take_into_account_score=True`), we will learn parameters by taking into account the prior specified by methods usePriorXXX () + the implicit prior of the score (if any). If `take_into_account_score=False`, we just take into account the prior specified by `usePriorXXX()`.

Returns
-------
pyagrum.BayesNet
	the learned BayesNet

Raises
------
pyagrum.MissingVariableInDatabase
	If a variable of the BN is not found in the database
pyagrum.UnknownLabelInDatabase
	If a label is found in the database that do not correspond to the variable
"


%feature("docstring") gum::learning::BNLearner::setPossibleEdges
"
Add a constraint by fixing the set of possible edges.

Parameters
----------
edges : Set[Tuple[int]]
	a set of edges as couples of nodeIds.
"

%feature("docstring") gum::learning::BNLearner::setPossibleSkeleton
"
Add a constraint by fixing the set of possible edges as a pyagrum.UndiGraph.

Parameters
----------
g : pyagrum.UndiGraph
	the fixed skeleton
"


%feature("docstring") gum::learning::BNLearner::setInitialDAG
"
Sets the initial structure (DAG) used by the structure learning algorithm.

Parameters
----------
dag : pyagrum.DAG
	an initial pyagrum.DAG structure
"


%feature("docstring") gum::learning::BNLearner::useEM
"
Sets whether we use EM for parameter learning or not, depending on the value of epsilon.

When EM is used, its stopping criterion is a threashold on the log-likelihood evolution rate.
If you wish to be more specific on which stopping criterion to use, you may prefer exploiting
methods useEMWithRateCriterion() or useEMWithDiffCriterion().

Parameters
----------
epsilon : float
	if epsilon>0 then EM is used and stops whenever the relative difference between two
    consecutive log-likelihoods (log-likelihood evolution rate) drops below epsilon.

	if epsilon=0.0 then EM is not used. But if you wish to forbid the use of EM, prefer
    executing Method forbidEM() rather than useEM(0) as it is more unequivocal.

noise: float (optional, default = 0.1)
        During EM's initialization, the CPTs are randomly perturbed using the following formula:
        new_CPT = (1-noise) * CPT + noise * random_CPT. Parameter noise must belong to interval [0,1].

        By default, noise is equal to 0.1.

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.

Raises
------
pyagrum.OutOfBounds
        if epsilon is strictly negative or if noise does not belong to interval [0,1].
"


%feature("docstring") gum::learning::BNLearner::useEMWithRateCriterion
"
Enforce that EM with the log-likelihood min evolution rate criterion will be used for
parameter learning if the dataset contains missing values.

Parameters
----------
epsilon : float
     epsilon sets the approximation stopping criterion: EM stops whenever the absolute
     value of the relative difference between two consecutive log-likelihoods drops below
     epsilon. Note that epsilon should be strictly positive.

noise: float (optional, default = 0.1)
        During EM's initialization, the CPTs are randomly perturbed using the following formula:
        new_CPT = (1-noise) * CPT + noise * random_CPT. Parameter noise must belong to interval [0,1].

        By default, noise is equal to 0.1.

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.

Raises
------
pyagrum.OutOfBounds
        if epsilon is not strictly positive or if noise does not belong to interval [0,1].
"


%feature("docstring") gum::learning::BNLearner::useEMWithDiffCriterion
"
Enforce that EM with the log-likelihood min difference criterion will be used for
parameter learning if the dataset contains missing values.

Parameters
----------
epsilon : float
     epsilon sets the approximation stopping criterion: EM stops whenever the
     difference between two consecutive log-likelihoods drops below
     epsilon. Note that epsilon should be strictly positive.

noise: float (optional, default = 0.1)
        During EM's initialization, the CPTs are randomly perturbed using the following formula:
        new_CPT = (1-noise) * CPT + noise * random_CPT. Parameter noise must belong to interval [0,1].

        By default, noise is equal to 0.1.

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.

Raises
------
pyagrum.OutOfBounds
        if epsilon is not strictly positive or if noise does not belong to interval [0,1]..


%feature("docstring") gum::learning::BNLearner::useMIIC
"
Indicate that we wish to use MIIC.
"

%feature("docstring") gum::learning::BNLearner::useNMLCorrection
"
Indicate that we wish to use the NML correction for MIIC
"

%feature("docstring") gum::learning::BNLearner::useMDLCorrection
"
Indicate that we wish to use the MDL correction for MIIC
"

%feature("docstring") gum::learning::BNLearner::useNoCorrection
"
Indicate that we wish to use the NoCorr correction for MIIC
"

%feature("docstring") gum::learning::IBNLearner::learnMixedStructure
"
Warnings
--------
learner must be using MIIC algorithm

Returns
-------
pyagrum.EssentialGraph
	the learned structure as an EssentialGraph
"

%feature("docstring") gum::learning::BNLearner::latentVariables
"
Warnings
--------
learner must be using MIIC algorithm

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
source : str|pyagrum.BayesNet
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
pyagrum.MissingVariableInDatabase
	If a variable of the BN is not found in the database.
"

%feature("docstring") gum::learning::IBNLearner::learnDAG
"
learn a structure from a file

Returns
-------
pyagrum.DAG
	the learned DAG
"


%feature("docstring") gum::learning::BNLearner::addNoChildrenNode
"
Add the constraint that this node cannot have any children.

Parameters
----------
node : int | str
  a variable's id or name
"

%feature("docstring") gum::learning::BNLearner::eraseNoChildrenNode
"
Remove the constraint that this node cannot have any children.

Parameters
----------
node : int | str
  a variable's id or name
"


%feature("docstring") gum::learning::BNLearner::addNoParentNode
"
Add the constraint that this node cannot have any parent.

Parameters
----------
node : int | str
  a variable's id or name
"

%feature("docstring") gum::learning::BNLearner::eraseNoParentNode
"
Remove the constraint that this node cannot have any parent.

Parameters
----------
node : int | str
  a variable's id or name
"

%feature("docstring") gum::learning::BNLearner::erasePossibleEdge
"
Allow the 2 arcs to be added if necessary.

Parameters
----------
arc : pyagrum.Arc
	an arc
head : int | str
	a variable's id or name
tail : int | str
	a variable's id or name
"
%feature("docstring") gum::learning::BNLearner::eraseForbiddenArc
"
Allow the arc to be added if necessary.

Parameters
----------
arc: pyagrum.Arc
	an arc
head : int | str
	a variable's id or name
tail : int | str
	a variable's id or name
"

%feature("docstring") gum::learning::BNLearner::eraseMandatoryArc
"
Parameters
----------
arc: pyagrum.Arc
	an arc
head : int | str
	a variable's id or name
tail : int | str
	a variable's id or name
"

%feature("docstring") gum::learning::BNLearner::addForbiddenArc
"
The arc in parameters won't be added.

Parameters
----------
arc : pyagrum.Arc
	an arc
head : int | str
	a variable's id or name
tail : int | str
	a variable's id or name
"

%feature("docstring") gum::learning::BNLearner::setForbiddenArcs
"
assign a set of forbidden arcs

Parameters
----------
arcs: Set[Tuple[int|str,int|str]]
"

%feature("docstring") gum::learning::BNLearner::setMandatoryArcs
"
assign a set of mandatory arcs

Parameters
----------
arcs: Set[Tuple[int|str,int|str]]
"

%feature("docstring") gum::learning::BNLearner::addMandatoryArc
"
Allow to add prior structural knowledge.

Parameters
----------
arc : pyagrum.Arc
	an arc
head : int | str
	a variable's id or name
tail : int | str
	a variable's id or name

Raises
------
pyagrum.InvalidDirectedCycle
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
arc : pyagrum.Arc
	an arc
head : int | str
	a variable's id or name
tail : int | str
	a variable's id or name
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
pyagrum.OutOfBounds
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
pyagrum.OutOfBounds
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
vars: List[int|str]
	the list of variables

Returns
-------
pyagrum.Tensor
	the pseudo-count as a Tensor
"

%feature("docstring") gum::learning::IBNLearner::rawPseudoCount
"
computes the pseudoCount (taking priors into account) of the list of variables as a list of floats.


Parameters
----------
vars: List[int|str]
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
computes the (log2) mutual information between two columns, given a list of other columns.

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
float
  the log2 mutual information
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

%feature("docstring") gum::learning::IBNLearner::nbRows
"
Return the number of row in the database


Returns
-------
int
	the number of rows in the database
"


%feature("docstring") gum::learning::IBNLearner::nbCols
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

%feature("docstring") gum::learning::BNLearner::isGumNumberOfThreadsOverriden
"
Check if the number of threads use by the learner is the default one or not.

Returns
-------
bool
	True if the number of threads used by the BNLearner has been set.
"


%feature("docstring") gum::learning::IBNLearner::setNumberOfThreads
"
If the parameter n passed in argument is different from 0, the BNLearner will use n threads during learning, hence overriding pyAgrum default number of threads.
If, on the contrary, n is equal to 0, the BNLearner will comply with pyAgrum default number of threads.

Parameters
----------
n : int
	the number of threads to be used by the BNLearner
"

%feature("docstring") gum::learning::IBNLearner::isGumNumberOfThreadsOverriden
"
Indicates whether the BNLearner currently overrides pyAgrum default number of threads (see method setNumberOfThreads).

Returns
-------
bool
	A Boolean indicating whether the BNLearner currently overrides pyAgrum default number of threads
"

%feature("docstring") gum::learning::IBNLearner::domainSize
"
Return the domain size of the variable with the given name.

Parameters
----------
n : str | int
  the name of the id of the variable
"

%feature("docstring") gum::learning::BNLearner::copyState
"
Copy the state of the given pyagrum.BNLearner (as argument).

Parameters
----------
pyagrum.BNLearner
    the learner whose state is copied.
"

%feature("docstring") gum::learning::BNLearner::isConstraintBased
"
Return wether the current learning method is constraint-based or not.

Returns
-------
bool
    True if the current learning method is constraint-based.
"

%feature("docstring") gum::learning::BNLearner::isScoreBased
"
Return wether the current learning method is score-based or not.

Returns
-------
bool
    True if the current learning method is score-based.
"
