%feature("docstring") gum::InfluenceDiagramInference::displayResult
"
Displays the result of an inference.
"

%feature("docstring") gum::InfluenceDiagramInference::displayStrongJunctionTree
"
Displays on terminal the result of strong junction tree computation for test purpose only.

Parameters
----------
args : TBW
"

%feature("docstring") gum::InfluenceDiagramInference::eraseAllEvidence
"
Removes all the evidence entered into the diagram.
"

%feature("docstring") gum::InfluenceDiagramInference::eraseEvidence
"
Parameters
----------
evidence : pyAgrum.Potential
	the evidence to remove

Warnings
--------
IndexError raised if the evidence does not belong to the influence diagram
"

%feature("docstring") gum::InfluenceDiagramInference::getBestDecisionChoice
"
Returns best choice for decision variable given in parameter ( based upon MEU criteria )

Parameters
----------
decisionId : int
	the id of the decision variable

Warnings
--------
OperationNotAllowed raised if no inference have yet been made

Warnings
--------
InvalidNode raised if node given in parmaeter is not a decision node 
"

%feature("docstring") gum::InfluenceDiagramInference::getMEU
"
Returns maximum expected utility obtained from inference.

Warnings
--------
OperationNotAllowed raised if no inference have yet been made 
"

%feature("docstring") gum::InfluenceDiagramInference::influenceDiagram
"
Returns a constant reference over the InfluenceDiagram on which this class work.

Returns
-------
pyAgrum.InfluenceDiagram
	the InfluenceDiagram on which this class work
"

%feature("docstring") gum::InfluenceDiagramInference::insertEvidence
"
Insert new evidence in the graph.

Parameters
----------
evidenceList : list
	a list of potentials as evidences

Warnings
--------
If an evidence already w.r.t. a given node and a new evidence w.r.t. this node is onserted, the old evidence is removed

Warnings
--------
OperationNotAllowed raised if an evidence is over more than one variable
"

%feature("docstring") gum::InfluenceDiagramInference::junctionTreeToDot
"
Returns
-------
str
	the result of strong junction tree computation for test purpose only.
"

%feature("docstring") gum::InfluenceDiagramInference::makeInference
"
Makes the inference. 
"