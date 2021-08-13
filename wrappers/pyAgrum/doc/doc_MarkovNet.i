%feature("docstring") gum::MarkovNet
"
MarkovNet represents a Markov Network.

MarkovNet(name='') -> MarkovNet
    Parameters:
      * **name** (*str*) -- the name of the Bayes Net

MarkovNet(source) -> MarkovNet
    Parameters:
      * **source** (*pyAgrum.MarkovNet*) -- the Markov network to copy
"

%feature("docstring") gum::MarkovNet::add
"
Add a variable to the pyAgrum.MarkovNet. 

Parameters
----------
variable : pyAgrum.DiscreteVariable
	the variable added
name : str 
	the variable name
nbrmod : int
	the number of modalities for the new variable
id : int
	the variable forced id in the pyAgrum.MarkovNet

Returns
-------
int 
	the id of the new node

Raises
------
pyAgrum.DuplicateLabel
    If variable.name() is already used in this pyAgrum.MarkovNet.
pyAgrum.NotAllowed
    If nbrmod is less than 2
pyAgrum.DuplicateElement
    If id is already used.
"

%feature("docstring") gum::MarkovNet::changeVariableName
"
Changes a variable's name in the gum::MarkovNet.

This will change the gum::DiscreteVariable names in the gum::MarkovNet.

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
pyAgrum.DuplicateLabel
    If new_name is already used in this MarkovNet.
pyAgrum.NotFound
    If no variable matches id.
"

%feature("docstring") gum::MarkovNet::changeVariableLabel
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
pyAgrum.NotFound
    if id/name is not a variable or if old_label does not exist.
"

%feature("docstring") gum::MarkovNet::endTopologyTransformation
"
Terminates a sequence of insertions/deletions of arcs by adjusting all CPTs dimensions.
End Multiple Change for all CPTs.

Returns
-------
pyAgrum.MarkovNet
"

%feature("docstring") gum::MarkovNet::clear
"
Clear the whole MarkovNet  
"

%feature("docstring") gum::MarkovNet::erase
"
Remove a variable from the gum::MarkovNet.

Removes the corresponding variable from the gum::MarkovNet and from all of it's children gum::Potential.

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

%feature("docstring") gum::MarkovNet::eraseArc
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

%feature("docstring") gum::MarkovNet::fastPrototype
"
Create a Markov network with a modified dot-like syntax which specifies:
    - the structure ``a-b-c;b-d-e;``. The substring ``a-b-c`` indicates a factor with the scope (a,b,c).
    - the type of the variables with different syntax (cf documentation).

Examples
--------
>>> import pyAgrum as gum
>>> bn=pyAgrum.MarkovNet.fastPrototype('A--B[1,3]-C{yes|No}--D[2,4]--E[1,2.5,3.9]',6)

Parameters
----------
dotlike : str
        the string containing the specification
domainSize : int
        the default domain size for variables

Returns
-------
pyAgrum.MarkovNet
        the resulting Markov network 
"

%feature("docstring") gum::MarkovNet::generateFactor
"
Randomly generate factor parameters for a given factor in a given structure.

Parameters
----------
node : int
	The variable's id.
name : str 
	The variable's name.
"

%feature("docstring") gum::MarkovNet::generateFactors
"
Randomly generates factors parameters for a given structure.
"

%feature("docstring") gum::MarkovNet::loadUAI
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
pyAgrum.IOError
    If file not found
pyAgrum.FatalError
    If file is not valid
"

%feature("docstring") gum::MarkovNet::saveUAI
"
Save the MarkovNet in an UAI file.

Parameters
----------
name : str
	the file's name
"

%feature("docstring") gum::MarkovNet::factor
"
Returns the factor of a set of variables (if existing).

Parameters
----------
VarId : Set[int]
	A variable's id in the pyAgrum.MarkovNet.
name : Set[str]
	A variable's name in the pyAgrum.MarkovNet.

Returns
-------
pyAgrum.Potential
	The factor of the set of nodes. 

Raises
------
pyAgrum.NotFound
    If no variable's id matches varId.
"
