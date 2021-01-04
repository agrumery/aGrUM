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
gum.IndexError
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
gum.OperationNotAllowed
	If no inference have yet been made
gum.InvalidNode
	If node given in parmaeter is not a decision node 
"

%feature("docstring") gum::ShaferShenoyLIMIDInference::MEU
"
Returns maximum expected utility obtained from inference.

Raises
------
gum.OperationNotAllowed
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
gum.OperationNotAllowed
	If an evidence is over more than one variable
"


%feature("docstring") gum::ShaferShenoyLIMIDInference::makeInference
"
Makes the inference. 
"