%define ADD_DOC(classname...)
%feature("docstring") gum::classname::BN
"
Returns
-------
pyAgrum.IBayesNet
  A constant reference over the IBayesNet referenced by this class.

Warnings
--------
UndefinedElement raised if no Bayes net has been assigned to the inference.
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
  Return the computed Shanon's entropy of a node given the observation.
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
  Return the computed Shanon's entropy of a node given the observation.
"

%feature("docstring") gum::classname::VI
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::addAllTargets
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::addEvidence
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::addJointTarget
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::addTarget
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::chgEvidence
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::eraseAllEvidence
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::eraseAllJointTargets
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::eraseAllMarginalTargets
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::eraseAllTargets
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::eraseEvidence
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::eraseJointTarget
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::eraseTarget
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::evidenceImpact
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::evidenceJointImpact
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::evidenceProbability
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::hardEvidenceNodes
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::hasEvidence
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::hasHardEvidence
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::hasSoftEvidence
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::isJointTarget
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::isTarget
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::joinTree
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::jointPosterior
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::jointTargets
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::junctionTree
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::makeInference
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::nbrEvidence
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::nbrHardEvidence
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::nbrJointTargets
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::nbrSoftEvidence
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::nbrTargets
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::posterior
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::setFindBarrenNodesType
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::setRelevantPotentialsFinderType
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::setTriangulation
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::softEvidenceNodes
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::targets
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::burnIn
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::isDrawnAtRandom
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::nbrDrawnVar
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::setBurnIn
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::setDrawnAtRandom
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::setNbrDrawnVar
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::currentPosterior
"
Warnings
--------
A faire in GenericInference
"

%feature("docstring") gum::classname::setVirtualLBPSize
"
Warnings
--------
A faire in GenericInference
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