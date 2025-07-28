%define ADD_GENERIC_INFERENCE_DOC(classname...)
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






%feature("docstring") gum::classname::BN
"
Returns
-------
pyagrum.IBayesNet
  A constant reference over the IBayesNet referenced by this class.

Raises
------
  pyagrum.UndefinedElement
    If no Bayes net has been assigned to the inference.
"

%feature("docstring") gum::classname::H
"
Parameters
----------
X : int
  a node Id
nodeName : str
  a node name

Returns
-------
float
  the computed Shanon's entropy of a node given the observation
"

%feature("docstring") gum::classname::I
"
Parameters
----------
X : int or str
   a node Id or a node name
Y : int or str
   another node Id or node name

   Returns
-------
float
   the Mutual Information of X and Y given the observation
"

%feature("docstring") gum::classname::VI
"
Parameters
----------
X : int or str
   a node Id or a node name
Y : int or str
   another node Id or node name

   Returns
-------
float
   variation of information between X and Y
"

%feature("docstring") gum::classname::addAllTargets
"
Add all the nodes as targets.
"

%feature("docstring") gum::classname::addEvidence
"
Adds a new evidence on a node (might be soft or hard).

Parameters
----------
id : int
  a node Id
nodeName : int
  a node name
val :
  (int) a node value
val :
  (str) the label of the node value
vals : list
  a list of values

Raises
------
  pyagrum.InvalidArgument
    If the node already has an evidence
  pyagrum.InvalidArgument
    If val is not a value for the node
  pyagrum.InvalidArgument
    If the size of vals is different from the domain side of the node
  pyagrum.FatalError
    If vals is a vector of 0s
  pyagrum.UndefinedElement
    If the node does not belong to the Bayesian network
"

%feature("docstring") gum::classname::addJointTarget
"
Add a list of nodes as a new joint target. As a collateral effect, every node is added as a marginal target.

Parameters
----------
list
  a list of names of nodes

Raises
------
pyagrum.UndefinedElement
  If some node(s) do not belong to the Bayesian network
"

%feature("docstring") gum::classname::addTarget
"
Add a marginal target to the list of targets.

Parameters
----------
target : int
  a node Id
nodeName : str
  a node name

Raises
------
pyagrum.UndefinedElement
  If target is not a NodeId in the Bayes net
"

%feature("docstring") gum::classname::chgEvidence
"
Change the value of an already existing evidence on a node (might be soft or hard).

Parameters
----------
id : int
  a node Id
nodeName : int
  a node name
val : int|str
  a node value or the label of the node value
vals : List[float]
  a list of values

Raises
------
pyagrum.InvalidArgument
  If the node does not already have an evidence
pyagrum.InvalidArgument
  If val is not a value for the node
pyagrum.InvalidArgument
  If the size of vals is different from the domain side of the node
pyagrum.FatalError
  If vals is a vector of 0s
pyagrum.UndefinedElement
  If the node does not belong to the Bayesian network
"

%feature("docstring") gum::classname::eraseAllEvidence
"
Removes all the evidence entered into the network.
"

%feature("docstring") gum::classname::eraseAllJointTargets
"
Clear all previously defined joint targets.
"

%feature("docstring") gum::classname::eraseAllMarginalTargets
"
Clear all the previously defined marginal targets.
"

%feature("docstring") gum::classname::eraseAllTargets
"
Clear all previously defined targets (marginal and joint targets).

As a result, no posterior can be computed (since we can only compute the posteriors of the marginal or joint targets that have been added by the user).
"

%feature("docstring") gum::classname::eraseEvidence
"
Remove the evidence, if any, corresponding to the node Id or name.

Parameters
----------
id : int
  a node Id
nodeName : int
  a node name

Raises
------
pyagrum.IndexError
  If the node does not belong to the Bayesian network
"

%feature("docstring") gum::classname::eraseJointTarget
"
Remove, if existing, the joint target.

Parameters
----------
list
  a list of names or Ids of nodes

Raises
------
pyagrum.IndexError
  If one of the node does not belong to the Bayesian network
pyagrum.UndefinedElement
  If node Id is not in the Bayesian network
"

%feature("docstring") gum::classname::eraseTarget
"
Remove, if existing, the marginal target.

Parameters
----------
target : int
  a node Id
nodeName : int
  a node name

Raises
------
pyagrum.IndexError
  If one of the node does not belong to the Bayesian network
pyagrum.UndefinedElement
  If node Id is not in the Bayesian network
"

%feature("docstring") gum::classname::evidenceImpact
"
Create a pyagrum.Tensor for P(target|evs) (for all instanciation of target and evs)

Parameters
----------
target : set
  a set of targets ids or names.
evs : set
  a set of nodes ids or names.

Warnings
--------
if some evs are d-separated, they are not included in the Tensor.

Returns
-------
pyagrum.Tensor
  a Tensor for P(targets|evs)
"

%feature("docstring") gum::classname::evidenceJointImpact
"
Create a pyagrum.Tensor for P(joint targets|evs) (for all instanciation of targets and evs)

Parameters
----------
targets : List[int|str]
  a list of node Ids or node names
evs : Set[int|str]
  a set of nodes ids or names.

Returns
-------
pyagrum.Tensor
  a Tensor for P(target|evs)

Raises
------
pyagrum.Exception
  If some evidene entered into the Bayes net are incompatible (their joint proba = 0)
"

%feature("docstring") gum::classname::evidenceProbability
"
Returns
-------
float
  the probability of evidence
"

%feature("docstring") gum::classname::hardEvidenceNodes
"
Returns
-------
set
  the set of nodes with hard evidence
"

%feature("docstring") gum::classname::hasEvidence
"
Parameters
----------
id : int
  a node Id
nodeName : str
  a node name

Returns
-------
bool
  True if some node(s) (or the one in parameters) have received evidence

Raises
------
pyagrum.IndexError
  If the node does not belong to the Bayesian network
"

%feature("docstring") gum::classname::hasHardEvidence
"
Parameters
----------
id : int
  a node Id
nodeName : str
  a node name

Returns
-------
bool
  True if node has received a hard evidence

Raises
------
pyagrum.IndexError
  If the node does not belong to the Bayesian network
"

%feature("docstring") gum::classname::hasSoftEvidence
"
Parameters
----------
id : int
  a node Id
nodeName : str
  a node name

Returns
-------
bool
  True if node has received a soft evidence

Raises
------
pyagrum.IndexError
  If the node does not belong to the Bayesian network
"

%feature("docstring") gum::classname::isJointTarget
"
Parameters
----------
list
  a list of nodes ids or names.

Returns
-------
bool
  True if target is a joint target.

Raises
------
pyagrum.IndexError
  If the node does not belong to the Bayesian network
pyagrum.UndefinedElement
  If node Id is not in the Bayesian network
"

%feature("docstring") gum::classname::isTarget
"
Parameters
----------
variable : int
 a node Id
nodeName : str
  a node name

Returns
-------
bool
  True if variable is a (marginal) target

Raises
------
pyagrum.IndexError
  If the node does not belong to the Bayesian network
pyagrum.UndefinedElement
  If node Id is not in the Bayesian network
"

%feature("docstring") gum::classname::joinTree
"
Returns
-------
pyagrum.CliqueGraph
  the current join tree used
"

%feature("docstring") gum::classname::jointPosterior
"
Compute the joint posterior of a set of nodes.

Parameters
----------
list :
  the list of nodes whose posterior joint probability is wanted


Warnings
--------
The order of the variables given by the list here or when the jointTarget is declared can not be assumed to be used by the Tensor.

Returns
-------
pyagrum.Tensor
  a const ref to the posterior joint probability of the set of nodes.

Raises
------
pyagrum.UndefinedElement
  If an element of nodes is not in targets
"

%feature("docstring") gum::classname::jointTargets
"
Returns
-------
list
  the list of target sets
"

%feature("docstring") gum::classname::junctionTree
"
Returns
-------
pyagrum.CliqueGraph
  the current junction tree
"

%feature("docstring") gum::classname::makeInference
"
Perform the heavy computations needed to compute the targets' posteriors

In a Junction tree propagation scheme, for instance, the heavy computations are those of the messages sent in the JT.
This is precisely what makeInference should compute. Later, the computations of the posteriors can be done 'lightly' by multiplying and projecting those messages.
"

%feature("docstring") gum::classname::nbrEvidence
"
Returns
-------
int
  the number of evidence entered into the Bayesian network
"

%feature("docstring") gum::classname::nbrHardEvidence
"
Returns
-------
int
  the number of hard evidence entered into the Bayesian network
"

%feature("docstring") gum::classname::nbrJointTargets
"
Returns
-------
int
  the number of joint targets
"

%feature("docstring") gum::classname::nbrSoftEvidence
"
Returns
-------
int
  the number of soft evidence entered into the Bayesian network
"

%feature("docstring") gum::classname::nbrTargets
"
Returns
-------
int
  the number of marginal targets
"

%feature("docstring") gum::classname::posterior
"
Computes and returns the posterior of a node.

Parameters
----------
var : int
  the node Id of the node for which we need a posterior probability
nodeName : str
  the node name of the node for which we need a posterior probability

Returns
-------
pyagrum.Tensor
  a const ref to the posterior probability of the node

Raises
------
pyagrum.UndefinedElement
  If an element of nodes is not in targets
"

%feature("docstring") gum::classname::softEvidenceNodes
"
Returns
-------
set
  the set of nodes with soft evidence
"

%feature("docstring") gum::classname::targets
"
Returns
-------
list
  the list of marginal targets
"

%feature("docstring") gum::classname::burnIn
"
Returns
-------
int
  size of burn in on number of iteration
"

%feature("docstring") gum::classname::isDrawnAtRandom
"
Returns
-------
bool
  True if variables are drawn at random
"

%feature("docstring") gum::classname::nbrDrawnVar
"
Returns
-------
int
  the number of variable drawn at each iteration
"

%feature("docstring") gum::classname::setBurnIn
"
Parameters
----------
b : int
  size of burn in on number of iteration
"

%feature("docstring") gum::classname::setDrawnAtRandom
"
Parameters
----------
_atRandom : bool
  indicates if variables should be drawn at random
"

%feature("docstring") gum::classname::setNbrDrawnVar
"
Parameters
----------
_nbr : int
  the number of variables to be drawn at each iteration
"

%feature("docstring") gum::classname::currentPosterior
"
Computes and returns the current posterior of a node.

Parameters
----------
var : int
  the node Id of the node for which we need a posterior probability
nodeName : str
  the node name of the node for which we need a posterior probability

Returns
-------
pyagrum.Tensor
  a const ref to the current posterior probability of the node

Raises
------
UndefinedElement
  If an element of nodes is not in targets
"

%feature("docstring") gum::classname::setVirtualLBPSize
"
Parameters
----------
vlbpsize : float
  the size of the virtual LBP
"

%feature("docstring") gum::classname::setRelevantTensorsFinderType
"
sets how we determine the relevant tensors to combine

When a clique sends a message to a separator, it first constitute the set of the tensors it contains and of the tensors contained in the messages it received. If RelevantTensorsFinderType = FIND_ALL, all these tensors are combined and projected to produce the message sent to the separator. If RelevantTensorsFinderType = DSEP_BAYESBALL_NODES, then only the set of tensors d-connected to the variables of the separator are kept for combination and projection.

0 = FIND_ALL
1 = DSEP_BAYESBALL_NODES
2 = DSEP_BAYESBALL_TENSORS
3 = DSEP_KOLLER_FRIEDMAN_2009

Parameters
----------
type : int
  the finder type

Raises
------
pyagrum.InvalidArgument
  If type is not implemented
"

%feature("docstring") gum::classname::setFindBarrenNodesType
"
sets how we determine barren nodes

Barren nodes are unnecessary for probability inference, so they can be safely discarded in this case (type = FIND_BARREN_NODES). This speeds-up inference. However, there are some cases in which we do not want to remove barren nodes, typically when we want to answer queries such as Most Probable Explanations (MPE).

0 = FIND_NO_BARREN_NODES
1 = FIND_BARREN_NODES

Parameters
----------
type : int
  the finder type

Raises
------
pyagrum.InvalidArgument
  If type is not implemented
"

%enddef


ADD_GENERIC_INFERENCE_DOC(LazyPropagation<double>)
ADD_GENERIC_INFERENCE_DOC(ShaferShenoyInference<double>)
ADD_GENERIC_INFERENCE_DOC(ShaferShenoyMRFInference<double>)
ADD_GENERIC_INFERENCE_DOC(VariableElimination<double>)
ADD_GENERIC_INFERENCE_DOC(GibbsSampling<double>)
ADD_GENERIC_INFERENCE_DOC(ImportanceSampling<double>)
ADD_GENERIC_INFERENCE_DOC(WeightedSampling<double>)
ADD_GENERIC_INFERENCE_DOC(MonteCarloSampling<double>)
ADD_GENERIC_INFERENCE_DOC(LoopyBeliefPropagation<double>)
ADD_GENERIC_INFERENCE_DOC(LoopySamplingInference<double,gum::GibbsSampling>)
ADD_GENERIC_INFERENCE_DOC(LoopySamplingInference<double,gum::ImportanceSampling>)
ADD_GENERIC_INFERENCE_DOC(LoopySamplingInference<double,gum::WeightedSampling>)
ADD_GENERIC_INFERENCE_DOC(LoopySamplingInference<double,gum::MonteCarloSampling>)


%define ADD_GENERIC_PARALLELIZED_INFERENCE_DOC(classname...)

%feature("docstring") gum::classname::setNumberOfThreads
"
If the argument nb is different from 0, this number of threads will be used during inferences, hence overriding aGrUM's default number of threads.
If, on the contrary, nb is equal to 0, the parallelized inference engine will comply with aGrUM's default number of threads.

Parameters
----------
nb : int
	the number of threads to be used by ShaferShenoyMRFInference
"

%feature("docstring") gum::classname::getNumberOfThreads
"
returns the number of threads used by LazyPropagation during inferences.

Returns
-------
int
	the number of threads used by LazyPropagation during inferences
"

%feature("docstring") gum::classname::isGumNumberOfThreadsOverriden
"
Indicates whether LazyPropagation currently overrides aGrUM's default number of threads (see method setNumberOfThreads).

Returns
-------
bool
	A Boolean indicating whether LazyPropagation currently overrides aGrUM's default number of threads
"

%feature("docstring") gum::classname::setMaxMemory
"
sets an upper bound on the memory consumption admissible

Parameters
----------
gigabytes: float
  this upper bound in gigabytes.
"

%enddef

ADD_GENERIC_PARALLELIZED_INFERENCE_DOC(LazyPropagation<double>)
ADD_GENERIC_PARALLELIZED_INFERENCE_DOC(ShaferShenoyInference<double>)
ADD_GENERIC_PARALLELIZED_INFERENCE_DOC(VariableElimination<double>)
ADD_GENERIC_PARALLELIZED_INFERENCE_DOC(ShaferShenoyMRFInference<double>)
