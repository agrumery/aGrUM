%feature("docstring") gum::BayesNet
"
BayesNet represents a Bayesian Network.

BayesNet(name='') -> BayesNet
    Parameters:
      * **name** (*str*) -- the name of the Bayes Net

BayesNet(source) -> BayesNet
    Parameters:
      * **source** (*pyAgrum.BayesNet*) -- the Bayesian network to copy
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
	the number of modalities for the new variable
id : int
	the variable forced id in the pyAgrum.BayesNet

Returns
-------
int 
	the id of the new node

Raises
------
gum.DuplicateLabel
    If variable.name() is already used in this pyAgrum.BayesNet.
gum.NotAllowed
    If nbrmod is less than 2
gum.DuplicateElement
    If id is already used.
"

%feature("docstring") gum::BayesNet::addAMPLITUDE
"
Others aggregators

Parameters
----------
variable : pyAgrum.DiscreteVariable
	the variable to be added

Returns
-------
int
	the id of the added value
"

%feature("docstring") gum::BayesNet::addAND
"
Add a variable, it's associate node and an AND implementation.

The id of the new variable is automatically generated.

Parameters
----------
variable : pyAgrum.DiscreteVariable
	The variable added by copy.

Returns
-------
int
	the id of the added variable. 

Raises
------
gum.SizeError
    If variable.domainSize()>2
"

%feature("docstring") gum::BayesNet::addArc
"
Add an arc in the BN, and update arc.head's CPT.

Parameters
----------
head :
	a variable's id (int)
tail :
	a variable's id (int)
head :
	a variable's name (str)
tail :
	a variable's name (str)

Raises
------
gum.InvalidEdge
    If arc.tail and/or arc.head are not in the BN.
gum.DuplicateElement
    If the arc already exists.
"

%feature("docstring") gum::BayesNet::addCOUNT
"
Others aggregators

Parameters
----------
variable : pyAgrum.DiscreteVariable
	the variable to be added

Returns
-------
int
	the id of the added value
"

%feature("docstring") gum::BayesNet::addEXISTS
"
Others aggregators

Parameters
----------
variable : pyAgrum.DiscreteVariable
	the variable to be added

Returns
-------
int
	the id of the added value
"

%feature("docstring") gum::BayesNet::addFORALL
"
Others aggregators

Parameters
----------
variable : pyAgrum.DiscreteVariable
	the variable to be added

Returns
-------
int
	the id of the added variable.
"

%feature("docstring") gum::BayesNet::addLogit
"
Add a variable, its associate node and a Logit implementation.

(The id of the new variable can be automatically generated.)

Parameters
----------
variable : pyAgrum.DiscreteVariable
	The variable added by copy
externalWeight : double
	the added external weight
id : int
	The proposed id for the variable.
Returns
-------
int
	the id of the added variable.

Raises
------
gum.DuplicateElement
    If id is already used
"

%feature("docstring") gum::BayesNet::addMAX
"
Others aggregators

Parameters
----------
variable : pyAgrum.DiscreteVariable
	the variable to be added

Returns
-------
int
	the id of the added value
"

%feature("docstring") gum::BayesNet::addMEDIAN
"
Others aggregators

Parameters
----------
variable : pyAgrum.DiscreteVariable
	the variable to be added

Returns
-------
int
	the id of the added value
"

%feature("docstring") gum::BayesNet::addMIN
"
Others aggregators

Parameters
----------
variable : pyAgrum.DiscreteVariable
	the variable to be added

Returns
-------
int
	the id of the added value
"

%feature("docstring") gum::BayesNet::addNoisyAND
"
Add a variable, its associate node and a noisyAND implementation.

(The id of the new variable can be automatically generated.)

Parameters
----------
variable : pyAgrum.DiscreteVariable
	The variable added by copy
externalWeight : double
	the added external weight
id : int
	The proposed id for the variable.

Returns
-------
int
	the id of the added variable.

Raises
------
gum.DuplicateElement
    If id is already used
"

%feature("docstring") gum::BayesNet::addNoisyOR
"
Add a variable, it's associate node and a noisyOR implementation.

Since it seems that the 'classical' noisyOR is the Compound noisyOR, we keep the addNoisyOR as an alias for addNoisyORCompound.

(The id of the new variable can be automatically generated.)

Parameters
----------
variable : pyAgrum.DiscreteVariable
	The variable added by copy
externalWeight : double
	the added external weight
id : int
	The proposed id for the variable.

Returns
-------
int
	the id of the added variable. 

Raises
--------
gum.DuplicateElement
    If id is already used
"

%feature("docstring") gum::BayesNet::addNoisyORCompound
"
Add a variable, it's associate node and a noisyOR implementation.

Since it seems that the 'classical' noisyOR is the Compound noisyOR, we keep the addNoisyOR as an alias for addNoisyORCompound.

(The id of the new variable can be automatically generated.)

Parameters
----------
variable : pyAgrum.DiscreteVariable
	The variable added by copy
externalWeight : double
	the added external weight
id : int
	The proposed id for the variable.

Returns
-------
int
	the id of the added variable.

Raises
--------
gum.DuplicateElement
    If id is already used
"

%feature("docstring") gum::BayesNet::addNoisyORNet
"
Add a variable, its associate node and a noisyOR implementation.

Since it seems that the 'classical' noisyOR is the Compound noisyOR, we keep the addNoisyOR as an alias for addNoisyORCompound.

(The id of the new variable can be automatically generated.)

Parameters
----------
variable : pyAgrum.DiscreteVariable
	The variable added by copy
externalWeight : double
	the added external weight
id : int
	The proposed id for the variable.

Returns
-------
int
	the id of the added variable.
"

%feature("docstring") gum::BayesNet::addOR
"
Add a variable, it's associate node and an OR implementation.

The id of the new variable is automatically generated.

Warnings
--------
	If parents are not boolean, all value>1 is True

Parameters
----------
variable : pyAgrum.DiscreteVariable
	The variable added by copy

Returns
-------
int
	the id of the added variable.

Raises
------
gum.SizeError
    If variable.domainSize()>2
"

%feature("docstring") gum::BayesNet::addWeightedArc
"
Add an arc in the BN, and update arc.head's CPT.

Parameters
----------
head :
	a variable's id (int)
tail :
	a variable's id (int)
head :
	a variable's name (str)
tail :
	a variable's name (str)
causalWeight : double
	the added causal weight

Raises
------
gum.InvalidArc
    If arc.tail and/or arc.head are not in the BN.
gum.InvalidArc
    If variable in arc.head is not a NoisyOR variable.
"

%feature("docstring") gum::BayesNet::beginTopologyTransformation
"
When inserting/removing arcs, node CPTs change their dimension with a cost in time.
begin Multiple Change for all CPTs
These functions delay the CPTs change to be done just once at the end of a sequence of topology modification, begins a sequence of insertions/deletions of arcs without changing the dimensions of the CPTs.
"

%feature("docstring") gum::BayesNet::changePotential
"
change the CPT associated to nodeId to newPot delete the old CPT associated to nodeId.

Parameters
----------
newPot : pyAgrum.Potential
	the new potential
NodeId : int
	the id of the node
name : str
	the name of the variable

Raises
------
gum.NotAllowed
    If newPot has not the same signature as __probaMap[NodeId]
"

%feature("docstring") gum::BayesNet::changeVariableName
"
Changes a variable's name in the gum::BayesNet.

This will change the gum::DiscreteVariable names in the gum::BayesNet.

Parameters
----------
new_name : str
	the new name of the variable
NodeId : int
	the id of the node
name : str
	the name of the variable

Raises
------
gum.DuplicateLabel
    If new_name is already used in this BayesNet.
gum.NotFound
    If no variable matches id.
"

%feature("docstring") gum::BayesNet::changeVariableLabel
"
change the label of the variable associated to nodeId to the new value.

Parameters
----------
id : int
	the id of the node
name : str
	the name of the variable
old_label : str
	the new label
new_label : str
	the new label

Raises
------
gum.NotFound
    if id/name is not a variable or if old_label does not exist.
"

%feature("docstring") gum::BayesNet::endTopologyTransformation
"
Terminates a sequence of insertions/deletions of arcs by adjusting all CPTs dimensions.
End Multiple Change for all CPTs.

Returns
-------
pyAgrum.BayesNet
"

%feature("docstring") gum::BayesNet::clear
"
Clear the whole BayesNet  
"

%feature("docstring") gum::BayesNet::erase
"
Remove a variable from the gum::BayesNet.

Removes the corresponding variable from the gum::BayesNet and from all of it's children gum::Potential.

If no variable matches the given id, then nothing is done.

Parameters
----------
id : int
	The variable's id to remove. 
name : str
	The variable's name to remove.
var : pyAgrum.DiscreteVariable
	A reference on the variable to remove. 
"

%feature("docstring") gum::BayesNet::eraseArc
"
Removes an arc in the BN, and update head's CTP.

If (tail, head) doesn't exist, the nothing happens.

Parameters
----------
arc : pyAgrum.Arc 
	The arc to be removed.
head :
	a variable's id (int)
tail :
	a variable's id (int)
head :
	a variable's name (str)
tail :
	a variable's name (str)
"

%feature("docstring") gum::BayesNet::fastPrototype
"
Create a Bayesian network with a dot-like syntax which specifies:
    - the structure 'a->b->c;b->d<-e;'.
    - the type of the variables with different syntax:

      - by default, a variable is a gum.RangeVariable using the default domain size (second argument)
      - with 'a[10]', the variable is a gum.RangeVariable using 10 as domain size (from 0 to 9)
      - with 'a[3,7]', the variable is a gum.RangeVariable using a domainSize from 3 to 7
      - with 'a[1,3.14,5,6.2]', the variable is a gum.DiscretizedVariable using the given ticks (at least 3 values)
      - with 'a{top|middle|bottom}', the variable is a gum.LabelizedVariable using the given labels.

Note 
----
  - If the dot-like string contains such a specification more than once for a variable, the first specification will be used.
  - the CPTs are randomly generated.
  - see also pyAgrum.fastBN.
    
Examples
--------
>>> import pyAgrum as gum
>>> bn=gum.BayesNet.fastPrototype('A->B[1,3]<-C{yes|No}->D[2,4]<-E[1,2.5,3.9]',6)

Parameters
----------
dotlike : str
        the string containing the specification
domainSize : int
        the default domain size for variables

Returns
-------
pyAgrum.BayesNet
        the resulting bayesian network 
"

%feature("docstring") gum::BayesNet::generateCPT
"
Randomly generate CPT for a given node in a given structure.

Parameters
----------
node : int
	The variable's id.
name : str 
	The variable's name.
"

%feature("docstring") gum::BayesNet::generateCPTs
"
Randomly generates CPTs for a given structure.
"

%feature("docstring") gum::BayesNet::loadBIF
"
Load a BIF file.

Parameters
----------
name : str
	the file's name
l : list
	list of functions to execute

Raises
--------
gum.IOError
    If file not found
gum.FatalError
    If file is not valid
"

%feature("docstring") gum::BayesNet::loadBIFXML
"
Load a BIFXML file.

Parameters
----------
name : str
	the name's file
l : list
	list of functions to execute

Raises
------
gum.IOError
    If file not found
gum.FatalError
    If file is not valid
"

%feature("docstring") gum::BayesNet::loadDSL
"
Load a DSL file.

Parameters
----------
name : str
	the file's name
l : list
	list of functions to execute

Raises
------
gum.IOError
    If file not found
gum.FatalError
    If file is not valid
"

%feature("docstring") gum::BayesNet::loadNET
"
Load a NET file.

Parameters
----------
name : str
	the name's file
l : list
	list of functions to execute

Raises
------
gum.IOError
    If file not found
gum.FatalError
    If file is not valid
"

%feature("docstring") gum::BayesNet::loadO3PRM
"
Load an O3PRM file.

Warnings
--------
The O3PRM language is the only language allowing to manipulate not only DiscretizedVariable but also RangeVariable and LabelizedVariable.

Parameters
----------
name : str
	the file's name
system : str
	the system's name
classpath : str
	the classpath
l : list
	list of functions to execute

Raises
------
gum.IOError
    If file not found
gum.FatalError
    If file is not valid
"

%feature("docstring") gum::BayesNet::loadUAI
"
Load an UAI file.

Parameters
----------
name : str
	the name's file
l : list
	list of functions to execute

Raises
------
gum.IOError
    If file not found
gum.FatalError
    If file is not valid
"

%feature("docstring") gum::BayesNet::reverseArc
"
Reverses an arc while preserving the same joint distribution. 

Parameters
----------
tail
	(int) the id of the tail variable
head
	(int) the id of the head variable
tail
	(str) the name of the tail variable
head
	(str) the name of the head variable
arc : pyAgrum.Arc
	an arc 

Raises
------
gum.InvalidArc
    If the arc does not exsit or if its reversal would induce a directed cycle.
"

%feature("docstring") gum::BayesNet::saveBIF
"
Save the BayesNet in a BIF file.

Parameters
----------
name : str
	the file's name
"

%feature("docstring") gum::BayesNet::saveBIFXML
"
Save the BayesNet in a BIFXML file.

Parameters
----------
name : str
	the file's name
"

%feature("docstring") gum::BayesNet::saveDSL
"
Save the BayesNet in a DSL file.

Parameters
----------
name : str
	the file's name
"

%feature("docstring") gum::BayesNet::saveNET
"
Save the BayesNet in a NET file.

Parameters
----------
name : str
	the file's name
"

%feature("docstring") gum::BayesNet::saveO3PRM
"
Save the BayesNet in an O3PRM file.

Warnings
--------
The O3PRM language is the only language allowing to manipulate not only DiscretizedVariable but also RangeVariable and LabelizedVariable.

Parameters
----------
name : str
	the file's name
"

%feature("docstring") gum::BayesNet::saveUAI
"
Save the BayesNet in an UAI file.

Parameters
----------
name : str
	the file's name
"

%feature("docstring") gum::BayesNet::cpt
"
Returns the CPT of a variable.

Parameters
----------
VarId : int
	A variable's id in the pyAgrum.BayesNet.
name : str
	A variable's name in the pyAgrum.BayesNet.

Returns
-------
pyAgrum.Potential
	The variable's CPT. 

Raises
------
gum.NotFound
    If no variable's id matches varId.
"
