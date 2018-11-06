%define ADD_DOC(classname...)
%feature("docstring") gum::classname::BN
"
Returns
-------
pyAgrum.IBayesNet
  A constant reference over the IBayesNet referenced by this class.

Raises
------
gum.UndefinedElement
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
double
  the computed Shanon's entropy of a node given the observation
"

%feature("docstring") gum::classname::I
"
Parameters
----------
X : int
  a node Id
Y : int
  another node Id

Returns
-------
double
  the computed Shanon's entropy of a node given the observation
"

%feature("docstring") gum::classname::VI
"
Parameters
----------
X : int
  a node Id
Y : int
  another node Id

Returns
-------
double
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
gum.InvalidArgument
  If the node already has an evidence
gum.InvalidArgument
  If val is not a value for the node
gum.InvalidArgument
  If the size of vals is different from the domain side of the node
gum.FatalError
  If vals is a vector of 0s
gum.UndefinedElement
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
gum.UndefinedElement
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
gum.UndefinedElement
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
val :
  (int) a node value
val :
  (str) the label of the node value
vals : list
  a list of values

Raises
------
gum.InvalidArgument
  If the node does not already have an evidence
gum.InvalidArgument
  If val is not a value for the node
gum.InvalidArgument
  If the size of vals is different from the domain side of the node
gum.FatalError
  If vals is a vector of 0s
gum.UndefinedElement
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
gum.IndexError
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
gum.IndexError
  If one of the node does not belong to the Bayesian network
gum.UndefinedElement
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
gum.IndexError
  If one of the node does not belong to the Bayesian network
gum.UndefinedElement
  If node Id is not in the Bayesian network
"

%feature("docstring") gum::classname::evidenceImpact
"
Create a pyAgrum.Potential for P(target|evs) (for all instanciation of target and evs)

Parameters
----------
target : set
  a set of targets ids or names.
evs : set
  a set of nodes ids or names.

Warnings
--------
if some evs are d-separated, they are not included in the Potential.

Returns
-------
pyAgrum.Potential
  a Potential for P(targets|evs)
"

%feature("docstring") gum::classname::evidenceJointImpact
"
Create a pyAgrum.Potential for P(joint targets|evs) (for all instanciation of targets and evs)

Parameters
----------
targets :
  (int) a node Id
targets :
  (str) a node name
evs : set
  a set of nodes ids or names.

Returns
-------
pyAgrum.Potential
  a Potential for P(target|evs)

Raises
------
gum.Exception
  If some evidene entered into the Bayes net are incompatible (their joint proba = 0)
"

%feature("docstring") gum::classname::evidenceProbability
"
Returns
-------
double
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
gum.IndexError
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
gum.IndexError
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
gum.IndexError
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
gum.IndexError
  If the node does not belong to the Bayesian network
gum.UndefinedElement
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
gum.IndexError
  If the node does not belong to the Bayesian network
gum.UndefinedElement
  If node Id is not in the Bayesian network
"

%feature("docstring") gum::classname::joinTree
"
Returns
-------
pyAgrum.CliqueGraph
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
The order of the variables given by the list here or when the jointTarget is declared can not be assumed to be used bu the Potential.

Returns
-------
pyAgrum.Potential
  a const ref to the posterior joint probability of the set of nodes.

Raises
------
gum.UndefinedElement
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
pyAgrum.CliqueGraph
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
pyAgrum.Potential
  a const ref to the posterior probability of the node

Raises
------
gum.UndefinedElement
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
pyAgrum.Potential
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
vlbpsize : double
  the size of the virtual LBP
"

%feature("docstring") gum::classname::asIApproximationSchemeConfiguration
"
Returns
-------
asIApproximationSchemeConfiguration
"

%feature("docstring") gum::classname::setRelevantPotentialsFinderType
"
sets how we determine the relevant potentials to combine

When a clique sends a message to a separator, it first constitute the set of the potentials it contains and of the potentials contained in the messages it received. If RelevantPotentialsFinderType = FIND_ALL, all these potentials are combined and projected to produce the message sent to the separator. If RelevantPotentialsFinderType = DSEP_BAYESBALL_NODES, then only the set of potentials d-connected to the variables of the separator are kept for combination and projection.

0 = FIND_ALL
1 = DSEP_BAYESBALL_NODES
2 = DSEP_BAYESBALL_POTENTIALS
3 = DSEP_KOLLER_FRIEDMAN_2009

Parameters
----------
type : int
  the finder type

Raises
------
gum.InvalidArgument
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
gum.InvalidArgument
  If type is not implemented
"

%enddef

ADD_DOC(LazyPropagation<double>)
ADD_DOC(ShaferShenoyInference<double>)
ADD_DOC(VariableElimination<double>)
ADD_DOC(GibbsSampling<double>)
ADD_DOC(ImportanceSampling<double>)
ADD_DOC(WeightedSampling<double>)
ADD_DOC(MonteCarloSampling<double>)
ADD_DOC(LoopyBeliefPropagation<double>)
ADD_DOC(LoopySamplingInference<double,gum::GibbsSampling>)
ADD_DOC(LoopySamplingInference<double,gum::ImportanceSampling>)
ADD_DOC(LoopySamplingInference<double,gum::WeightedSampling>)
ADD_DOC(LoopySamplingInference<double,gum::MonteCarloSampling>)
