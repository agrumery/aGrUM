%feature("docstring") gum::BayesNet
"
BayesNet represents a Bayesian Network.

Available constructors:
    ``BayesNet(name='') -> BayesNet``

    ``BayesNet(source) -> BayesNet``

Parameters
----------
name: str
  the name of the Bayes Net (optional)
source : pyAgrum.BayesNet
  the Bayesian network to copy
"

%feature("docstring") gum::BayesNet::add
"
Add a variable to the pyAgrum.BayesNet. 

Parameters
----------
variable : pyAgrum.DiscreteVariable
	the variable added
name : str 
	the variable name
nbrmod : int
	the numbre of modalities for the new variable
id : int
	the variable forced id in the pyAgrum.BayesNet

Returns
-------
int 
	the id of the new node

Warnings
--------
DuplicateLabel raised if variable.name() is already used in this pyAgrum.BayesNet. 

Warnings
--------
NotAllowed if nbrmod<2

Warnings
--------
DuplicateElement raised if id is already used. 
"

%feature("docstring") gum::BayesNet::addAMPLITUDE
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::addAND
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::addArc
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::addCOUNT
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::addEXISTS
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::addFORALL
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::addLogit
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::addMAX
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::addMEDIAN
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::addMIN
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::addNoisyAND
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::addNoisyOR
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::addNoisyORCompound
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::addNoisyORNet
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::addOR
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::addWeightedArc
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::arcs
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::beginTopologyTransformation
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::changePotential
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::changeVariableName
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::children
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::cpt
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::endTopologyTransformation
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::erase
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::eraseArc
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::fastPrototype
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::generateCPT
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::generateCPTs
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::idFromName
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::ids
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::loadBIF
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::loadBIFXML
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::loadDSL
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::loadNET
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::loadO3PRM
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::loadUAI
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::minimalCondSet
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::moralGraph
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::names
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::nodeId
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::parents
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::reverseArc
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::saveBIF
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::saveBIFXML
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::saveDSL
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::saveNET
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::saveO3PRM
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::saveUAI
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::variable
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::variableFromName
"
Warnings
--------
A Faire
"

%feature("docstring") gum::BayesNet::variableNodeMap
"
Warnings
--------
A Faire
"
