%feature("docstring") gum::InfluenceDiagram
"
InfluenceDiagram represents an Influence Diagram.

Available constructors:
    ``InfluenceDiagram() -> InfluenceDiagram``

    ``InfluenceDiagram(source) -> InfluenceDiagram``

Parameters
----------
source : pyAgrum.InfluenceDiagram
  the InfluenceDiagram to copy
"

%feature("docstring") gum::InfluenceDiagram::add
"
Add a chance variable, it's associate node and it's CPT.

The id of the new variable is automatically generated.

Parameters
----------
variable : pyAgrum.DiscreteVariable
	The variable added by copy.
id : int
	The chosen id. If 0, the NodeGraphPart will choose.

Warnings
--------
give an id (not 0) should be reserved for rare and specific situations !!! 

Returns
-------
int
    the id of the added variable. 

Raises
------
gum.DuplicateElement
	If id(<>0) is already used 
"

%feature("docstring") gum::InfluenceDiagram::addArc
"
Add an arc in the ID, and update diagram's potential nodes cpt if necessary.

Parameters
----------
tail : int
  the id of the tail node
head : int
  the id of the head node

Raises
------
gum.InvalidEdge
	If arc.tail and/or arc.head are not in the ID.
gum.InvalidEdge
	If tail is a utility node 
"

%feature("docstring") gum::InfluenceDiagram::addChanceNode
"
Add a chance variable, it's associate node and it's CPT.

The id of the new variable is automatically generated.

Parameters
----------
variable : pyAgrum.DiscreteVariable
	the variable added by copy.
id : int
	the chosen id. If 0, the NodeGraphPart will choose.

Warnings
--------
give an id (not 0) should be reserved for rare and specific situations !!! 

Returns
-------
int
    the id of the added variable. 

Raises
------
gum.DuplicateElement
	If id(<>0) is already used 
"

%feature("docstring") gum::InfluenceDiagram::addDecisionNode
"
Add a decision variable.

The id of the new variable is automatically generated.

Parameters
----------
variable : pyAgrum.DiscreteVariable
	the variable added by copy.
id : int
	the chosen id. If 0, the NodeGraphPart will choose.

Warnings
--------
give an id (not 0) should be reserved for rare and specific situations !!! 

Returns
-------
int
    the id of the added variable. 

Raises
------
gum.DuplicateElement
	If id(<>0) is already used 
"

%feature("docstring") gum::InfluenceDiagram::addUtilityNode
"
Add a utility variable, it's associate node and it's UT. 

The id of the new variable is automatically generated.

Parameters
----------
variable : pyAgrum.DiscreteVariable
	the variable added by copy
id : int
	the chosen id. If 0, the NodeGraphPart will choose

Warnings
--------
give an id (not 0) should be reserved for rare and specific situations !!! 

Returns
-------
int
    the id of the added variable. 

Raises
------
gum.InvalidArgument
	If variable has more than one label
gum.DuplicateElement
	If id(<>0) is already used 
"

%feature("docstring") gum::InfluenceDiagram::arcs
"
Returns
-------
list:
	the list of all the arcs in the Influence Diagram.
"

%feature("docstring") gum::InfluenceDiagram::chanceNodeSize
"
Returns
-------
int
	the number of chance nodes. 
"

%feature("docstring") gum::InfluenceDiagram::changeVariableName
"
Parameters
----------
id : int
	the node Id
new_name : str
	the name of the variable 

Raises
------
gum.DuplicateLabel
	If this name already exists
gum.NotFound
	If no nodes matches id. 
"

%feature("docstring") gum::InfluenceDiagram::children
"
Parameters
----------
id : int
  the id of the parent

Returns
-------
Set
	the set of all the children
"

%feature("docstring") gum::InfluenceDiagram::cpt
"
Returns the CPT of a variable.

Parameters
----------
VarId : int
	A variable's id in the pyAgrum.BayesNet.

Returns
-------
pyAgrum.Potential
	The variable's CPT. 

Raises
------
gum.NotFound
	If no variable's id matches varId. 
"

%feature("docstring") gum::InfluenceDiagram::decisionNodeSize
"
Returns
-------
int
	the number of decision nodes
"

%feature("docstring") gum::InfluenceDiagram::decisionOrderExists
"
Returns
-------
bool
	True if a directed path exist with all decision node
"

%feature("docstring") gum::InfluenceDiagram::erase
"
Erase a Variable from the network and remove the variable from all his childs.

If no variable matches the id, then nothing is done.

Parameters
----------
id : int
	The id of the variable to erase.
var : pyAgrum.DiscreteVariable
	The reference on the variable to remove. 
"

%feature("docstring") gum::InfluenceDiagram::eraseArc
"
Removes an arc in the ID, and update diagram's potential nodes cpt if necessary.

If (tail, head) doesn't exist, the nothing happens.

Parameters
----------
arc : pyAgrum.Arc
	The arc to be removed.
tail : int
  the id of the tail node
head : int
  the id of the head node
"

%feature("docstring") gum::InfluenceDiagram::existsPathBetween
"
Returns
-------
bool
	true if a path exists between two nodes. 
"

%feature("docstring") gum::InfluenceDiagram::getDecisionGraph
"
Returns
-------
pyAgrum.DAG
	the temporal Graph. 
"

%feature("docstring") gum::InfluenceDiagram::getDecisionOrder
"
Returns
-------
list
	the sequence of decision nodes in the directed path.

Raises
------
NotFound
	If such a path does not exist 
"

%feature("docstring") gum::InfluenceDiagram::idFromName
"
Returns a variable's id given its name.

Parameters
----------
name : str
	the variable's name from which the id is returned.

Returns
-------
int
	the variable's node id. 

Raises
------
gum.NotFound
	If no such name exists in the graph. 
"

%feature("docstring") gum::InfluenceDiagram::ids
"
Returns
-------
list
	The list variables ids.
"

%feature("docstring") gum::InfluenceDiagram::isChanceNode
"
Parameters
----------
varId : int 
	the tested node id.

Returns
-------
bool
	true if node is a chance node
"

%feature("docstring") gum::InfluenceDiagram::isDecisionNode
"
Parameters
----------
varId : int 
	the tested node id.

Returns
-------
bool
	true if node is a decision node
"

%feature("docstring") gum::InfluenceDiagram::isUtilityNode
"
Parameters
----------
varId : int 
	the tested node id.

Returns
-------
bool
	true if node is an utility node
"

%feature("docstring") gum::InfluenceDiagram::loadBIFXML
"
Load a BIFXML file.

Parameters
----------
name : str
	the name's file

Raises
------
gum.IOError
	If file not found
gum.FatalError
	If file is not valid
"

%feature("docstring") gum::InfluenceDiagram::names
"
Returns
-------
list
	The names of the InfluenceDiagram variables
"


%feature("docstring") gum::InfluenceDiagram::nodeId
"
Parameters
----------
var : pyAgrum.DiscreteVariable
	a variable

Returns
-------
int
	the id of the variable

Raises
------
gum.IndexError
	If the InfluenceDiagram does not contain the variable
"

%feature("docstring") gum::InfluenceDiagram::parents
"
Parameters
----------
id :
	The id of the child node

Returns
-------
set
    the set of the parents ids.
"

%feature("docstring") gum::InfluenceDiagram::saveBIFXML
"
Save the BayesNet in a BIFXML file.

Parameters
----------
name : str
	the file's name
"

%feature("docstring") gum::InfluenceDiagram::toDot
"
Returns
-------
str
    a friendly display of the graph in DOT format
"

%feature("docstring") gum::InfluenceDiagram::utility
"
Parameters
----------
varId : int 
	the tested node id.

Returns
-------
pyAgrum.Potential
	the utility table of the node

Raises
------
gum.IndexError
	If the InfluenceDiagram does not contain the variable
"

%feature("docstring") gum::InfluenceDiagram::utilityNodeSize
"
Returns
-------
int
	the number of utility nodes
"

%feature("docstring") gum::InfluenceDiagram::variable
"
Parameters
----------
id : int
 	the node id

Returns
------
pyAgrum.DiscreteVariable
	a constant reference over a variabe given it's node id

Raises
------
gum.NotFound
	If no variable's id matches the parameter 
"

%feature("docstring") gum::InfluenceDiagram::variableFromName
"
Parameters
----------
name : str
	a variable's name

Returns
-------
pyAgrum.DiscreteVariable
	the variable

Raises
------
gum.IndexError
	If the InfluenceDiagram does not contain the variable
"

%feature("docstring") gum::InfluenceDiagram::variableNodeMap
"
TBD
"
