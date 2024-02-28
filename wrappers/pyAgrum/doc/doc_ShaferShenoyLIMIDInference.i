%feature("docstring") gum::ShaferShenoyLIMIDInference
"
This inference considers the provided model as a LIMID rather than an influence diagram. It is an optimized
implementation of the LIMID resolution algorithm. However an inference on a classical influence diagram can be performed
by adding a assumption of the existence of the sequence of decision nodes to be solved, which also implies that the
decision choices can have an impact on the rest of the sequence (Non Forgetting Assumption,
cf. pyAgrum.ShaferShenoyLIMIDInference.addNoForgettingAssumption).
"

%feature("docstring") gum::ShaferShenoyLIMIDInference::eraseAllEvidence
"
Removes all the evidence entered into the diagram.
"

%feature("docstring") gum::ShaferShenoyLIMIDInference::eraseEvidence
"
Parameters
----------
evidence : pyAgrum.Potential
	the evidence to remove

Raises
------
  pyAgrum.IndexError
	If the evidence does not belong to the influence diagram
"

%feature("docstring") gum::ShaferShenoyLIMIDInference::optimalDecision
"
Returns best choice for decision variable given in parameter ( based upon MEU criteria )

Parameters
----------
decisionId : int,str
	the id or name of the decision variable

Raises
------
  pyAgrum.OperationNotAllowed
	If no inference have yet been made
pyAgrum.InvalidNode
	If node given in parmaeter is not a decision node
"

%feature("docstring") gum::ShaferShenoyLIMIDInference::MEU
"
Returns maximum expected utility obtained from inference.

Raises
------
pyAgrum.OperationNotAllowed
	If no inference have yet been made
"

%feature("docstring") gum::ShaferShenoyLIMIDInference::influenceDiagram
"
Returns a constant reference over the InfluenceDiagram on which this class work.

Returns
-------
pyAgrum.InfluenceDiagram
	the InfluenceDiagram on which this class work
"

%feature("docstring") gum::ShaferShenoyLIMIDInference::insertEvidence
"
Insert new evidence in the graph.

Parameters
----------
evidenceList : list
	a list of potentials as evidences

Warnings
--------
If an evidence already w.r.t. a given node and a new evidence w.r.t. this node is onserted, the old evidence is removed

Raises
------
pyAgrum.OperationNotAllowed
	If an evidence is over more than one variable
"


%feature("docstring") gum::ShaferShenoyLIMIDInference::makeInference
"
Makes the inference.
"


%feature("docstring") gum::ShaferShenoyLIMIDInference::addEvidence
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
  pyAgrum.InvalidArgument
    If the node already has an evidence
  pyAgrum.InvalidArgument
    If val is not a value for the node
  pyAgrum.InvalidArgument
    If the size of vals is different from the domain side of the node
  pyAgrum.FatalError
    If vals is a vector of 0s
  pyAgrum.UndefinedElement
    If the node does not belong to the Bayesian network
"


%feature("docstring") gum::ShaferShenoyLIMIDInference::chgEvidence
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
pyAgrum.InvalidArgument
  If the node does not already have an evidence
pyAgrum.InvalidArgument
  If val is not a value for the node
pyAgrum.InvalidArgument
  If the size of vals is different from the domain side of the node
pyAgrum.FatalError
  If vals is a vector of 0s
pyAgrum.UndefinedElement
  If the node does not belong to the Bayesian network
"

%feature("docstring") gum::ShaferShenoyLIMIDInference::eraseAllEvidence
"
Remove all evidence.
"



%feature("docstring") gum::ShaferShenoyLIMIDInference::eraseEvidence
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
pyAgrum.IndexError
  If the node does not belong to the Bayesian network
"


%feature("docstring") gum::ShaferShenoyLIMIDInference::hardEvidenceNodes
"
Returns
-------
set
  the set of nodes with hard evidence
"


%feature("docstring") gum::ShaferShenoyLIMIDInference::hasEvidence
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
pyAgrum.IndexError
  If the node does not belong to the Bayesian network
"

%feature("docstring") gum::ShaferShenoyLIMIDInference::hasHardEvidence
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
pyAgrum.IndexError
  If the node does not belong to the Bayesian network
"

%feature("docstring") gum::ShaferShenoyLIMIDInference::hasSoftEvidence
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
pyAgrum.IndexError
  If the node does not belong to the Bayesian network
"


%feature("docstring") gum::ShaferShenoyLIMIDInference::isSolvable
"
check wether the influence diagram is solvable or not

Returns
-------
bool
  True if the influence diagram is solvable
"


%feature("docstring") gum::ShaferShenoyLIMIDInference::junctionTree
"
Returns
-------
pyAgrum.CliqueGraph
  the current junction tree
"

%feature("docstring") gum::ShaferShenoyLIMIDInference::makeInference
"
Perform the heavy computations needed to compute the optimal decisions.
"

%feature("docstring") gum::ShaferShenoyLIMIDInference::makeInference
"
Perform the heavy computations needed to compute the optimal decisions.
"


%feature("docstring") gum::ShaferShenoyLIMIDInference::meanVar
"
Parameters
----------
id : int
  a node Id
nodeName : str
  a node name
  
Returns
-------
dict[str, float]
  a dictionary with the mean and variance of the node (after the inference)
"


%feature("docstring") gum::ShaferShenoyLIMIDInference::nbrEvidence
"
Returns
-------
int
  the number of evidence entered into the Bayesian network
"

%feature("docstring") gum::ShaferShenoyLIMIDInference::nbrHardEvidence
"
Returns
-------
int
  the number of hard evidence entered into the Bayesian network
"

%feature("docstring") gum::ShaferShenoyLIMIDInference::nbrSoftEvidence
"
Returns
-------
int
  the number of soft evidence entered into the Bayesian network
"

%feature("docstring") gum::ShaferShenoyLIMIDInference::posterior
"
Returns the posterior of a chance or a decision node (after optimisation).

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
"

%feature("docstring") gum::ShaferShenoyLIMIDInference::posteriorUtility
"
Returns the posterior utiliyt of a utility node (after optimisation) depending on decision nodes, if any.

Parameters
----------
var : int
  the node Id of the node for which we need a posterior probability
nodeName : str
  the node name of the node for which we need a posterior probability

Returns
-------
pyAgrum.Potential
  a const ref to the posterior utility of the utility node

"

%feature("docstring") gum::ShaferShenoyLIMIDInference::reducedGraph
"
Returns the DAG build to solve the influence diagram.

Returns
-------
pyAgrum.DAG
  a copy of the reduced graph

"

%feature("docstring") gum::ShaferShenoyLIMIDInference::reducedLIMID
"
Returns the (reduced) LIMID build to solve the influence diagram.

Returns
-------
pyAgrum.InfluenceDiagram
  a copy of the reduced influence Diagram (LIMID)

"