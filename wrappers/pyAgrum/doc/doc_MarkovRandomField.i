%feature("docstring") gum::MarkovRandomField
"
MarkovRandomField represents a Markov random field.

MarkovRandomField(name='') -> MarkovRandomField
    Parameters:
      - **name** (*str*) -- the name of the Bayes Net

MarkovRandomField(source) -> MarkovRandomField
    Parameters:
      - **source** (*pyAgrum.MarkovRandomField*) -- the Markov random field to copy
"

%feature("docstring") gum::MarkovRandomField::add
"
Add a variable to the pyAgrum.MarkovRandomField.

Parameters
----------
variable : pyAgrum.DiscreteVariable
	the variable added
name : str
	the variable name
nbrmod : int
	the number of modalities for the new variable
id : int
	the variable forced id in the pyAgrum.MarkovRandomField

Returns
-------
int
	the id of the new node

Raises
------
pyAgrum.DuplicateLabel
    If variable.name() is already used in this pyAgrum.MarkovRandomField.
pyAgrum.OperationNotAllowed
    If nbrmod is less than 2
pyAgrum.DuplicateElement
    If id is already used.
"

%feature("docstring") gum::MarkovRandomField::addFactor
"
Add a factor from a list or a set of id or str. If the argument is a set, the order is the order of the IDs of the variables

Parameters
----------
seq : sequence (list or set) of int or string
	The sequence (ordered or not) of node id or names
"

%feature("docstring") gum::MarkovRandomField::changeVariableName
"
Changes a variable's name in the gum::MarkovRandomField.

This will change the gum::DiscreteVariable names in the gum::MarkovRandomField.

Parameters
----------
car :Union[int,str]
	a variable's id (int) or name
new_name : str
	the new name of the variable

Raises
------
pyAgrum.DuplicateLabel
    If new_name is already used in this MarkovRandomField.
pyAgrum.NotFound
    If no variable matches id.
"

%feature("docstring") gum::MarkovRandomField::changeVariableLabel
"
change the label of the variable associated to nodeId to the new value.

Parameters
----------
var :Union[int,str]
	a variable's id (int) or name
old_label : str
	the old label
new_label : str
	the new label

Raises
------
pyAgrum.NotFound
    if id/name is not a variable or if old_label does not exist.
"

%feature("docstring") gum::MarkovRandomField::endTopologyTransformation
"
Terminates a sequence of insertions/deletions of arcs by adjusting all CPTs dimensions.
End Multiple Change for all CPTs.

Returns
-------
pyAgrum.MarkovRandomField
"

%feature("docstring") gum::MarkovRandomField::clear
"
Clear the whole MarkovRandomField
"

%feature("docstring") gum::MarkovRandomField::erase
"
Remove a variable from the gum::MarkovRandomField.

Removes the corresponding variable from the gum::MarkovRandomField and from all of it's children gum::Potential.

If no variable matches the given id, then nothing is done.

Parameters
----------
var :Union[int,str,pyAgrum.DiscreteVariable]
	a variable's id (int) or name of variable or a reference of this variable to remove.
"

%feature("docstring") gum::MarkovRandomField::eraseArc
"
Removes an arc in the BN, and update head's CTP.

If (tail, head) doesn't exist, the nothing happens.

Parameters
----------
arc : pyAgrum.Arc
	The arc to be removed.
head : Union[int,str]
	a variable's id (int) or name
tail : Union[int,str]
	a variable's id (int) or name
"

%feature("docstring") gum::MarkovRandomField::fastPrototype
"
Create a Markov random field with a modified dot-like syntax which specifies:
    - the structure ``a-b-c;b-d-e;``. The substring ``a-b-c`` indicates a factor with the scope (a,b,c).
    - the type of the variables with different syntax (cf documentation).

Examples
--------
>>> import pyAgrum as gum
>>> bn=pyAgrum.MarkovRandomField.fastPrototype('A--B[1,3]-C{yes|No}--D[2,4]--E[1,2.5,3.9]',6)

Parameters
----------
dotlike : str
        the string containing the specification
domainSize : int or str
        the default domain size or the default domain for variables

Returns
-------
pyAgrum.MarkovRandomField
        the resulting Markov random field
"

%feature("docstring") gum::MarkovRandomField::generateFactor
"
Randomly generate factor parameters for a given factor in a given structure.

Parameters
----------
node : Union[int,str]
	a variable's id (int) or name
"

%feature("docstring") gum::MarkovRandomField::generateFactors
"
Randomly generates factors parameters for a given structure.
"

%feature("docstring") gum::MarkovRandomField::loadUAI
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

%feature("docstring") gum::MarkovRandomField::saveUAI
"
Save the MarkovRandomField in an UAI file.

Parameters
----------
name : str
	the file's name
"

%feature("docstring") gum::MarkovRandomField::factor
"
Returns the factor of a set of variables (if existing).

Parameters
----------
vars : Union[Set[int],Set[str]]
	A set of ids or names of variable the pyAgrum.MarkovRandomField.

Returns
-------
pyAgrum.Potential
	The factor of the set of nodes.

Raises
------
pyAgrum.NotFound
    If no variable's id matches varId.
"
