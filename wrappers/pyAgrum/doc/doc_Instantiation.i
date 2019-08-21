%feature("docstring") gum::Instantiation
"
Class for assigning/browsing values to tuples of discrete variables.

Instantiation is designed to assign values to tuples of variables and to efficiently loop over values of subsets of variables.

Instantiation() -> Instantiation

Instantiation(aI) -> Instantiation
    Parameters:
      * **aI** (*pyAgrum.Instantiation*) -- the Instantiation we copy

Returns
-------
pyAgrum.Instantiation
	An empty tuple or a copy of the one in parameters

Instantiation is subscriptable therefore values can be easily accessed/modified.

Examples
--------
>>> ## Access the value of A in an instantiation aI
>>> valueOfA = aI['A']
>>> ## Modify the value
>>> aI['A'] = newValueOfA
"

%feature("docstring") gum::Instantiation::add
"
Adds a new variable in the Instantiation.

Parameters
----------
v : pyAgrum.DiscreteVariable
  The new variable added to the Instantiation

Raises
------
DuplicateElement
  If the variable is already in this Instantiation

"

%feature("docstring") gum::Instantiation::chgVal
"
Assign newval to v (or to the variable at position varPos) in the Instantiation.

Parameters
----------
v : pyAgrum.DiscreteVariable or string
  The variable whose value is assigned (or its name)
varPos : int
  The index of the variable whose value is assigned in the tuple of variables of the Instantiation
newval : int or string
  The index of the value assigned (or its name)

Returns
-------
pyAgrum.Instantiation
    The modified instantiation

Raises
------
NotFound
  If variable v does not belong to the instantiation.
OutOfBound
  If newval is not a possible value for the variable.
"

%feature("docstring") gum::Instantiation::clear
"
Erase all variables from an Instantiation.
"

%feature("docstring") gum::Instantiation::contains
"
Indicates whether a given variable belongs to the Instantiation.

Parameters
----------
v : pyAgrum.DiscreteVariable
    The variable for which the test is made.

Returns
-------
bool :
    True if the variable is in the Instantiation.
"

%feature("docstring") gum::Instantiation::dec
"
Operator --.
"

%feature("docstring") gum::Instantiation::decIn
"
Operator -- for the variables in i.

Parameters
----------
i : pyAgrum.Instantiation
  The set of variables to decrement in this Instantiation
"

%feature("docstring") gum::Instantiation::decNotVar
"
Operator -- for vars which are not v.

Parameters
----------
v : pyAgrum.DiscreteVariable
  The variable not to decrement in this Instantiation.
"

%feature("docstring") gum::Instantiation::decOut
"
Operator -- for the variables not in i.

Parameters
----------
i : pyAgrum.Instantiation
  The set of variables to not decrement in this Instantiation.
"

%feature("docstring") gum::Instantiation::decVar
"
Operator -- for variable v only.

Parameters
----------
v : pyAgrum.DiscreteVariable
 The variable to decrement in this Instantiation.

Raises
------
NotFound
  If variable v does not belong to the Instantiation.
"

%feature("docstring") gum::Instantiation::domainSize
"
Returns
-------
int
    The product of the variable's domain size in the Instantiation.
"

%feature("docstring") gum::Instantiation::empty
"
Returns
-------
bool
    True if the instantiation is empty.
"

%feature("docstring") gum::Instantiation::end
"
Returns
-------
bool
    True if the Instantiation reached the end.
"

%feature("docstring") gum::Instantiation::erase
"
Parameters
----------
v : pyAgrum.DiscreteVariable
  The variable to be removed from this Instantiation.

Raises
------
NotFound
  If v does not belong to this Instantiation.
"

%feature("docstring") gum::Instantiation::hamming
"
Returns
-------
int
  the hamming distance of this instantiation.
"

%feature("docstring") gum::Instantiation::inOverflow
"
Returns
-------
bool
  True if the current value of the tuple is correct
"

%feature("docstring") gum::Instantiation::inc
"
Operator ++.
"

%feature("docstring") gum::Instantiation::incIn
"
Operator ++ for the variables in i.

Parameters
----------
i : pyAgrum.Instantiation
    The set of variables to increment in this Instantiation.
"

%feature("docstring") gum::Instantiation::incNotVar
"
Operator ++ for vars which are not v.

Parameters
----------
v : pyAgrum.DiscreteVariable
    The variable not to increment in this Instantiation.
"

%feature("docstring") gum::Instantiation::incOut
"
Operator ++ for the variables not in i.

Parameters
----------
i : Instantiation
    The set of variable to not increment in this Instantiation.
"

%feature("docstring") gum::Instantiation::incVar
"
Operator ++ for variable v only.

Parameters
----------
v : pyAgrum.DiscreteVariable
    The variable to increment in this Instantiation.

Raises
------
NotFound
  If variable v does not belong to the Instantiation.
"

%feature("docstring") gum::Instantiation::nbrDim
"
Returns
-------
int
    The number of variables in the Instantiation.
"

%feature("docstring") gum::Instantiation::pos
"
Returns
-------
int
   the position of the variable v.

Parameters
----------
v : pyAgrum.DiscreteVariable
    the variable for which its position is return.

Raises
------
NotFound
  If v does not belong to the instantiation.
"

%feature("docstring") gum::Instantiation::rend
"
Returns
-------
bool:
  True if the Instantiation reached the rend.
"

%feature("docstring") gum::Instantiation::reorder
"
Reorder vars of this instantiation giving the order in v (or i).

Parameters
----------
i : pyAgrum.Instantiation
  The sequence of variables with which to reorder this Instantiation.
v : list
    The new order of variables for this Instantiation.
"

%feature("docstring") gum::Instantiation::setFirst
"
Assign the first values to the tuple of the Instantiation.
"

%feature("docstring") gum::Instantiation::setFirstIn
"
Assign the first values in the Instantiation for the variables in i.

Parameters
----------
i : pyAgrum.Instantiation
  The variables to which their first value is assigned in this Instantiation.
"

%feature("docstring") gum::Instantiation::setFirstNotVar
"
Assign the first values to variables different of v.

Parameters
----------
v : pyAgrum.DiscreteVariable
  The variable that will not be set to its first value in this Instantiation.
"

%feature("docstring") gum::Instantiation::setFirstOut
"
Assign the first values in the Instantiation for the variables not in i.

Parameters
----------
i : pyAgrum.Instantiation
    The variable that will not be set to their first value in this Instantiation.
"

%feature("docstring") gum::Instantiation::setFirstVar
"
Assign the first value in the Instantiation for var v.

Parameters
----------
v : pyAgrum.DiscreteVariable
    The variable that will be set to its first value in this Instantiation.
"

%feature("docstring") gum::Instantiation::setLast
"
Assign the last values in the Instantiation.
"

%feature("docstring") gum::Instantiation::setLastIn
"
Assign the last values in the Instantiation for the variables in i.

Parameters
----------
i : pyAgrum.Instantiation
    The variables to which their last value is assigned in this Instantiation.
"

%feature("docstring") gum::Instantiation::setLastNotVar
"
Assign the last values to variables different of v.

Parameters
----------
v : pyAgrum.DiscreteVariable
    The variable that will not be set to its last value in this Instantiation.
"

%feature("docstring") gum::Instantiation::setLastOut
"
Assign the last values in the Instantiation for the variables not in i.

Parameters
----------
i : pyAgrum.Instantiation
    The variables that will not be set to their last value in this Instantiation.
"

%feature("docstring") gum::Instantiation::setLastVar
"
Assign the last value in the Instantiation for var v.

Parameters
----------
v : pyAgrum.DiscreteVariable
  The variable that will be set to its last value in this Instantiation.
"

%feature("docstring") gum::Instantiation::setVals
"
Assign the values from i in the Instantiation.

Parameters
----------
i : pyAgrum.Instantiation
  An Instantiation in which the new values are searched

Returns
-------
pyAgrum.Instantiation
  a reference to the instantiation
"

%feature("docstring") gum::Instantiation::unsetEnd
"
Alias for unsetOverflow().
"

%feature("docstring") gum::Instantiation::unsetOverflow
"
Removes the flag overflow.
"

%feature("docstring") gum::Instantiation::val
"
Parameters
----------
i : int
	The index of the variable.
var : pyAgrum.DiscreteVariable
	The variable the value of which we wish to know

Returns
-------
int
	the current value of the variable.

Raises
------
NotFound
  If the element cannot be found.
"

%feature("docstring") gum::Instantiation::variable
"
Parameters
----------
i : int
  The index of the variable

Returns
-------
pyAgrum.DiscreteVariable
  the variable at position i in the tuple.

Raises
------
NotFound
  If the element cannot be found.
"

%feature("docstring") gum::Instantiation::variablesSequence
"
Returns
-------
List
    the sequence of DiscreteVariable of this instantiation.
"

%feature("docstring") gum::Instantiation::todict
"
Create a dict (variable_name:value) from an instantiation

Parameters
----------
withLabels : boolean
	The value will be a label (string) if True. It will be a position (int) if False.

Returns
-------
Dict
    The dictionary
"

%feature("docstring") gum::Instantiation::fromdict
"
Change the values in an instantiation from a dict (variable_name:value) where value can be a position (int) or a label (string).

If a variable_name does not occur in the instantiation, nothing is done.

Warnings
--------
    OutOfBounds raised if a value cannot be found.
"
