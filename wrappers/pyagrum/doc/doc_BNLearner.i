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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
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


/// ================================================================================
/// perform learning
/// ================================================================================

%feature("docstring") gum::learning::BNLearner::learnBN
"
Learns a BayesNet (both parameters and structure) from the BNLearner's database

Returns
-------
pyagrum.BayesNet
	the learnt BayesNet
"


%feature("docstring") gum::learning::IBNLearner::learnDAG
"
learn a structure from a file

Returns
-------
pyagrum.DAG
	the learned DAG
"


%feature("docstring") gum::learning::IBNLearner::learnPDAG
"
learn a partially directed acyclic graph (PDAG) from the BNLearner's database

Returns
-------
pyagrum.PDAG
	the learned PDAG

Warnings
--------
  The learning method must be constraint-based (MIIC, etc.) and not score-based
  (K2, GreedyHillClimbing, etc.)
"


%feature("docstring") gum::learning::BNLearner::learnParameters
"
Creates a Bayes net whose structure corresponds to that passed in argument or to
the last structure learnt by Method `learnDAG()`, and whose parameters are learnt
from the BNLearner's database.

usage:
  1. `learnParameters(dag, take_into_account_score=True)`
  2. `learnParameters(bn, take_into_account_score=True)`
  3. `learnParameters(take_into_account_score=True)`

When the first argument of Method `learnParameters()` is a DAG or a Bayes net
(usages 1. and 2.), this one specifies the graphical structure of the returned
Bayes net. Otherwise (usage 3.), Method `learnParameters()` is called implicitly
with the last DAG learnt by the BNLearner.

The difference between calling this method with a DAG (usages 1. and 3.) or a
Bayes net (usage 2.) arises when the database contains missing values and EM is
used to learn the parameters. EM needs to initialize the conditional probability
distributions (CPT) before iterating the expectation/maximimzation steps. When a
DAG is passed in argument, these initializations are performed using a specific
estimator that does not take into account the missing values in the database. The
resulting CPTs are then perturbed randomly (see the noise in method `useEM()`).
When a Bayes net is passed in argument, its CPT for a node A can be either filled
exclusively with zeroes or not. In the first case, the initialization is performed
as described above. In the second case, the value of A's CPT is used as is, and
a subsequent perturbation controlled by the noise level is applied.

Parameters
----------
dag : pyagrum.DAG
	specifies the graphical structure of the returned Bayes net.
bn : pyagrum.BayesNet
	specifies the graphical structure of the returned Bayes net and, when
	the database contains missing values and EM is used for learning, force
	EM to initialize the CPTs of the resulting Bayes net to the values of
	those passed in argument (when they are not fully filled with zeroes)
	before iterating over the expectation/maximization steps.
take_into_account_score : bool, default=True
	The graphical structure passed in argument may have been learnt from a
	structure learning. In this case, if the score used to learn the structure
	has an implicit prior (like K2 which has a 1-smoothing prior), it is important
	to also take into account this implicit prior for parameter learning. By
	default (`take_into_account_score=True`), we will learn parameters by taking
	into account the prior specified by methods usePriorXXX() + the implicit prior
	of the score (if any). If `take_into_account_score=False`, we just take into
	account the prior specified by `usePriorXXX()`.

Returns
-------
pyagrum.BayesNet
	the learnt BayesNet

Raises
------
pyagrum.MissingVariableInDatabase
	If a variable of the Bayes net is not found in the database
pyagrum.MissingValueInDatabase
	If the database contains some missing values and EM is not used for the learning
pyagrum.OperationNotAllowed
	If EM is used but no stopping criterion has been selected
pyagrum.UnknownLabelInDatabase
	If a label is found in the database that do not correspond to the variable

Warnings
--------
When using a `pyagrum.DAG` as input parameter, the NodeIds in the dag and index of
rows in the database must fit in order to coherently fix the structure of the BN.
Generally, it is safer to use a `pyagrum.BayesNet` as input or even to use
`pyagrum.BNLearner.fitParameters`.
"


%feature("docstring") gum::learning::BNLearner::fitParameters
"
fitParameters directly populates the CPTs of the Bayes Net (bn) passed in argument using the
database and the structure of bn.

When the database contains missing values and a method among useEM(), useEMWithRateCriterion()
and useEMWithDiffCriterion(), has been executed, the CPTs are learnt using the EM algorithm.
This one first initializes the CPTs and, then, iterates sequences
of expectations/maximizations in order to converge toward a most likely CPT. The aforementioned
initialization is performed as follows: when a CPT contains only zeroes, the BNLearner estimates
it from the database without taking into account the missing values, else it uses the
CPT as is. Then, in both cases, the CPT is randomly perturbed using the following formula:
new_CPT = (1-alpha) * CPT + alpha * random_CPT. Parameter alpha is called a noise factor and
can be controlled using Method useEM().

Parameters
----------
bn : pyagrum.BayesNet
	a BN which will directly have its parameters learned inplace.

take_into_account_score : bool
  The dag passed in argument may have been learnt from a structure learning. In this case,
  if the score used to learn the structure has an implicit prior (like K2 which has a 1-smoothing prior),
  it is important to also take into account this implicit prior for parameter learning. By default
  (`take_into_account_score=True`), we will learn parameters by taking into account the prior specified
  by methods usePriorXXX () + the implicit prior of the score (if any). If `take_into_account_score=False`,
  we just take into account the prior specified by `usePriorXXX()`.
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


/// ================================================================================
/// Selecting the structure learning algorithm
/// ================================================================================

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


%feature("docstring") gum::learning::BNLearner::useMIIC
"
Indicate that we wish to use MIIC.
"


%feature("docstring") gum::learning::BNLearner::isScoreBased
"
Return wether the current learning method is score-based or not.

Returns
-------
bool
    True if the current learning method is score-based.
"


%feature("docstring") gum::learning::BNLearner::isConstraintBased
"
Return wether the current learning method is constraint-based or not.

Returns
-------
bool
    True if the current learning method is constraint-based.
"


/// ================================================================================
/// Scores and priors (for structure learning)
/// ================================================================================

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


/// ================================================================================
/// Managing structure learning constraints
/// ================================================================================

%feature("docstring") gum::learning::BNLearner::addNoChildrenNode
"
Add to structure learning algorithms the constraint that this node cannot have any children.

Parameters
----------
node : int | str
	a variable's id or name
"

%feature("docstring") gum::learning::BNLearner::eraseNoChildrenNode
"
Remove in structure learning algorithms the constraint that this node cannot have any children.

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
Forbid the arc passed in argument to be added during structure learning
(methods `learnDAG()` or `learnBN()`).

Usage:
  1. addForbiddenArc(tail, head)
  2. addForbiddenArc(arc)

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
Allow an arc to be added if necessary during structure learning
(methods `learnDAG()` or `learnBN()`).

Usage:
  1. addMandatoryArc(tail, head)
  2. addMandatoryArc(arc)

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
By default, all edge is possible. However, once at least one possible edge is defined,
all other edges not declared possible are considered as impossible.

Parameters
----------
arc : pyagrum.Arc
	an arc
head : int | str
	a variable's id or name
tail : int | str
	a variable's id or name
"


%feature("docstring") gum::learning::BNLearner::setSliceOrder
"
Set a partial order on the nodes.

Parameters
----------
l : list
        a list of sequences (composed of ids of rows or string)
"


%feature("docstring") gum::learning::BNLearner::setMaxIndegree
"
Parameters
----------
max_indegree : int
	the limit number of parents
"



/// ================================================================================
/// EM parameter learning fine-tuning
/// ================================================================================


%feature("docstring") gum::learning::BNLearner::useEM
"
Sets whether we use EM for parameter learning or not, depending on the value of epsilon.

usage:
  * `useEM(epsilon, noise=0.1)`

When epsilon is equal to 0.0, EM is forbidden, else EM is used for parameter learning
whenever the database contains missing values. In this case, its stopping criterion
is a threshold on the log-likelihood evolution rate, i.e., if llc and llo refer to
the log-likelihoods at the current and previous EM steps respectively, EM will stop
when (llc - llo) / llc drops below epsilon. If you wish to be more specific on which
stopping criterion to use, you may prefer exploiting methods `useEMWithRateCriterion()`
or `useEMWithDiffCriterion()`.

Parameters
----------
epsilon : float
	if epsilon>0 then EM is used and stops whenever the relative difference between two
	consecutive log-likelihoods (log-likelihood evolution rate) drops below epsilon.

	if epsilon=0.0 then EM is not used. But if you wish to forbid the use of EM, prefer
	executing Method `forbidEM()` rather than useEM(0.0) as it is more unequivocal.

noise: float, default=0.1
        During EM's initialization, the CPTs are randomly perturbed using the following formula:
        new_CPT = (1-noise) * CPT + noise * random_CPT. Parameter noise must belong
        to interval [0,1]. By default, noise is equal to 0.1.

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.

Raises
------
pyagrum.OutOfBounds
        if `epsilon` is strictly negative or if `noise` does not belong to interval [0,1].
"


%feature("docstring") gum::learning::BNLearner::useEMWithRateCriterion
"
Enforces that EM with the log-likelihood min evolution rate stopping criterion will be
used for parameter learning when the dataset contains missing values.

Parameters
----------
epsilon : float
     epsilon sets the approximation stopping criterion: EM stops whenever the absolute
     value of the relative difference between two consecutive log-likelihoods drops below
     epsilon. Note that epsilon should be strictly positive.

noise: float, default=0.1
        During EM's initialization, the CPTs are randomly perturbed using the following formula:
        new_CPT = (1-noise) * CPT + noise * random_CPT. Parameter noise must belong
        to interval [0,1]. By default, noise is equal to 0.1.

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.

Raises
------
pyagrum.OutOfBounds
        if `epsilon` is not strictly positive or if `noise` does not belong to interval [0,1].
"


%feature("docstring") gum::learning::BNLearner::useEMWithDiffCriterion
"
Enforces that EM with the log-likelihood min difference criterion will be used for
parameter learning whenever the dataset contains missing values.

Parameters
----------
epsilon : float
     epsilon sets the approximation stopping criterion: EM stops whenever the
     difference between two consecutive log-likelihoods drops below
     epsilon. Note that epsilon should be strictly positive.

noise: float (optional, default = 0.1)
        During EM's initialization, the CPTs are randomly perturbed using the following formula:
        new_CPT = (1-noise) * CPT + noise * random_CPT. Parameter noise must belong
        to interval [0,1]. By default, noise is equal to 0.1.

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.

Raises
------
pyagrum.OutOfBounds
        if epsilon is not strictly positive or if noise does not belong to interval [0,1].
"


%feature("docstring") gum::learning::BNLearner::forbidEM
"
Forbids the use of EM for parameter learning.

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.
"


%feature("docstring") gum::learning::IBNLearner::isUsingEM
"
returns a Boolean indicating whether EM is used for parameter learning when
the database contains missing values.
"


%feature("docstring") gum::learning::BNLearner::EMsetEpsilon
"
Enforces that the minimal difference between two consecutive log-likelihoods
is chosen as a stopping criterion of the EM parameter learning algorithm and
specifies the threshold on this criterion.

Parameters
----------
eps: float
        the log-likelihood difference below which EM stops its iterations

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.

Raises
------
pyagrum.OutOfBounds
        If eps <= 0.
"


%feature("docstring") gum::learning::BNLearner::EMdisableEpsilon
"
Disables the minimal difference between two consecutive log-likelihoods as a
stopping criterion for the EM parameter learning algorithm.

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.
"


%feature("docstring") gum::learning::BNLearner::EMenableEpsilon
"
Enforces that the minimal difference between two consecutive log-likelihoods is
a stopping criterion for the EM parameter learning algorithm.

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.

Warnings:
---------
Setting the min difference between two consecutive log-likelihoods as a stopping
criterion disables the min log-likelihood evolution rate as a stopping criterion.
"


%feature("docstring") gum::learning::IBNLearner::EMEpsilon
"
Returns a float corresponding to the minimal difference between two consecutive
log-likelihoods under which the EM parameter learning algorithm stops.

Returns
-------
float
       the minimal difference between two consecutive log-likelihoods under which EM stops.
"


%feature("docstring") gum::learning::IBNLearner::EMisEnabledEpsilon
"
Returns a Boolean indicating whether the minimal difference between two consecutive
log-likelihoods is a stopping criterion for the EM parameter learning algorithm.
"


%feature("docstring") gum::learning::BNLearner::EMsetMinEpsilonRate
"
Enforces that the minimal log-likelihood's evolution rate is considered by the EM
parameter learning algorithm as a stopping criterion.

Parameters
----------
rate: float
	the log-likelihood evolution rate below which EM stops its iterations

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.

Raises
------
pyagrum.OutOfBounds
	If rate <= 0.

Warnings
--------
Setting this stopping criterion disables the min log-likelihod difference criterion
(if this one was enabled).
"


%feature("docstring") gum::learning::BNLearner::EMdisableMinEpsilonRate
"
Disables the minimal log-likelihood's evolution rate as an EM parameter learning
stopping criterion.

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.
"


%feature("docstring") gum::learning::BNLearner::EMenableMinEpsilonRate
"
Enables the minimal log-likelihood's evolution rate as an EM parameter learning
stopping criterion.

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.

Warnings
--------
Setting this stopping criterion disables the min log-likelihod difference criterion.
"


%feature("docstring") gum::learning::IBNLearner::EMMinEpsilonRate
"
Returns a float corresponding to the minimal log-likelihood's evolution rate under
which the EM parameter learning algorithm stops its iterations.

Returns
-------
float
	the limit under which EM stops its expectation/maximization iterations
"


%feature("docstring") gum::learning::IBNLearner::EMisEnabledMinEpsilonRate
"
Returns a Boolean indicating whether the minimal log-likelihood's evolution rate is
considered as a stopping criterion by the EM parameter learning algorithm.
"


%feature("docstring") gum::learning::BNLearner::EMsetMaxIter
"
Enforces a limit on the number of expectation/maximization steps performed by EM.

Parameters
----------
max : int
	the maximal number of iterations that EM is allowed to perform

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.

Raises
------
pyagrum.OutOfBounds
	If max <= 1.
"


%feature("docstring") gum::learning::BNLearner::EMdisableMaxIter
"
Do not limit EM to perform a maximal number of iterations.

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.
"


%feature("docstring") gum::learning::BNLearner::EMenableMaxIter
"
Enables a limit on the number of iterations performed by EM. This number is
equal to the last number specified with Method `EMsetMaxIter()`.
See Method `EMMaxIter()` to get its current value.

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.
"


%feature("docstring") gum::learning::IBNLearner::EMMaxIter
"
Returns an int containing the max number of iterations the EM parameter learning
algorithm is allowed to perform when the max iterations stopping criterion is enabled.

Returns
-------
float
	the max number of expectation/maximization iterations EM is allowed to perform
"


%feature("docstring") gum::learning::IBNLearner::EMisEnabledMaxIter
"
Returns a Boolean indicating whether the max number of iterations is used
by EM as a stopping criterion.
"


%feature("docstring") gum::learning::BNLearner::EMsetMaxTime
"
Adds a constraint on the time that EM is allowed to run for learning
parameters.

Parameters
----------
timeout : float
	the timeout in milliseconds

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.

Raises
------
pyagrum.OutOfBounds
        If timeout<=0.0
"


%feature("docstring") gum::learning::BNLearner::EMdisableMaxTime
"
Allow EM to learn parameters for an infinite amount of time.

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.
"


%feature("docstring") gum::learning::BNLearner::EMenableMaxTime
"
Forbid EM to run more than a given amount of time.

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.
"


%feature("docstring") gum::learning::IBNLearner::EMMaxTime
"
Returns a float indicating EM's time limit when the max time stopping
criterion is used by the EM parameter learning algorithm.

Returns
-------
float
        the max time EM is allowed to execute its expectation/maximization iterations
"


%feature("docstring") gum::learning::IBNLearner::EMisEnabledMaxTime
"
Returns a Boolean indicating whether the max time criterion is used as
an EM stopping criterion.
"


%feature("docstring") gum::learning::BNLearner::EMsetVerbosity
"
Sets or unsets the verbosity of the EM parameter learning algorithm.

Verbosity is necessary for keeping track of the history of the learning.
See Method `EMHistory()`.

Parameters
----------
v : bool
	sets EM's verbose mode if and only if v = True.

Returns
-------
pyagrum.BNLearner
        the BNLearner itself, so that we can chain useXXX() methods.
"


%feature("docstring") gum::learning::IBNLearner::EMVerbosity
"
Returns a Boolean indicating whether the EM parameter learning algorithm
is in a verbose mode.

Note that EM verbosity is necessary for recording the history of the
log-likelihoods computed at each expectation/maximization step.

Returns
-------
bool
        indicates whether EM's verbose mode is active or not
"


%feature("docstring") gum::learning::BNLearner::EMState
"
Returns a message specifying the current state of the EM parameter learning algorithm.

See Method `EMStateApproximationScheme` for more details. To make it short, it may be
interesting, when EM has completed, to determine for which reason it stopped, i.e.,
which stopping criterion made it stop. In particular, although it is fortunately quite
seldom, it can be stopped due the log-likelihood diverging (decreasing) instead of
converging toward a fixed point (see Table 5 on p28 of
https://faculty.washington.edu/fxia/courses/LING572/EM_collins97.pdf).

Returns
-------
str
        a string containing the state of EM.
"


%feature("docstring") gum::learning::BNLearner::EMStateApproximationScheme
"
Returns an int representing the state of the EM algorithm.

The possible states are:
  0. Undefined
  1. Continue: EM has not been stopped yet
  2. Epsilon: EM has been stopped due to the min log-likelihood difference stopping criterion
  3. Rate: EM has been stopped due to the min log-likelihood evolution rate stopping criterion
  4. Limit: EM has been stopped due to a limit on the number of iterations
  5. TimeLimit: EM has been stopped due to a timeout
  6. Stopped: EM has been stopped because there was a divergence on the log-likelihood
"


%feature("docstring") gum::learning::IBNLearner::EMnbrIterations
"
Returns the number of iterations performed by the EM parameter learning algorithm.
"


%feature("docstring") gum::learning::IBNLearner::EMHistory
"
Returns a list containing the log-likelihoods recorded after each
expectation/maximization iteration of the EM parameter learning algorithm.

Returns
-------
List[float]
        A list of all the log-likelihoods recorded during EM's execution

Warnings
--------
Recording log-likelihoods is enabled only when EM is executed in verbose
mode. See method `EMsetVerbosity()`.
"



/// ================================================================================
/// Database inspection / requests
/// ================================================================================

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


%feature("docstring") gum::learning::IBNLearner::domainSize
"
Return the domain size of the variable with the given name.

Parameters
----------
n : str | int
  the name of the id of the variable
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
chi2 computes the chi2 statistic and p-value of two variables conditionally to a
list of other variables.

The variables correspond to columns in the database and are specified as the
names of these columns in the database. The list of variables in the conditioning
set can be empty. In this case, no need to specify it.

Usage:
  * `chi2(name1, name2, knowing=[])`

Parameters
----------
name1: str
        the name of a variable/column in the database

name2 : str
        the name/column of another variable

knowing : List[str]
        the list of the column names of the conditioning variables

Returns
-------
Tuple[float,float]
        the chi2 statistics and the associated p-value as a Tuple
"


%feature("docstring") gum::learning::BNLearner::G2
"
G2 computes the G2 statistic and p-value of two variables conditionally to a
list of other variables.

The variables correspond to columns in the database and are specified as the
names of these columns in the database. The list of variables in the conditioning
set can be empty. In this case, no need to specify it.

Usage:
  * `G2(name1, name2, knowing=[])`

Parameters
----------
name1: str
        the name of a variable/column in the database

name2 : str
        the name/column of another variable

knowing : List[str]
        the list of the column names of the conditioning variables

Returns
-------
Tuple[float,float]
        the G2 statistics and the corresponding p-value as a Tuple
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




























%feature("docstring") gum::learning::BNLearner::state
"
Returns a dictionary containing the current state of the BNLearner.

Returns
-------
Dict[str,Any]
    a dictionary containing the current state of the BNLearner.
"







%feature("docstring") gum::learning::BNLearner::setPossibleEdges
"
Adds a constraint to the structure learning algorithm by fixing the set of possible edges.

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



/// ================================================================================
/// Fine-tuning the behavior of the BNLearner
/// ================================================================================

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


%feature("docstring") gum::learning::BNLearner::copyState
"
Copy the state of the given pyagrum.BNLearner (as argument).

Parameters
----------
pyagrum.BNLearner
    the learner whose state is copied.
"
