%ignore gum::DisceteVariable::clone;

%feature("docstring") gum::DiscreteVariable
"
DiscreteVariable is the base class for discrete random variable.

Available constructors:

    ``DiscreteVariable(aName, aDesc='') -> DiscreteVariable``

    ``DiscreteVariable(aDRV) -> DiscreteVariable``

Parameters
----------
aName: str
  The name of the variable
aDesc: str
  The (optional) description of the variable
aDRV: pyAgrum.DiscreteVariable
  Another `DiscreteVariable` that will be copied
"

%feature("docstring") gum::DiscreteVariable::clone
"
Returns
-------
pyAgrum.DiscreteVariable
	a copy of the DiscreteVariable
"

%feature("docstring") gum::DiscreteVariable::domain
"
Returns
-------
str
	the domain of the variable
"

%feature("docstring") gum::DiscreteVariable::domainSize
"
Returns
-------
int
	the number of modalities in the variable domain
"

%feature("docstring") gum::DiscreteVariable::empty
"
Returns
-------
bool
	True if the domain size < 2
"

%feature("docstring") gum::DiscreteVariable::index
"
Parameters
----------
label : str
	a label

Returns
-------
int
	the indice of the label
"

%feature("docstring") gum::DiscreteVariable::label
"
Parameters
----------
i : int
	the index of the label we wish to return

Returns
-------
str
	the indice-th label

Raises
------
gum.OutOfBound
	If the variable does not contain the label
"

%feature("docstring") gum::DiscreteVariable::labels
"
Returns
-------
tuple
	a tuple containing the labels
"

%feature("docstring") gum::DiscreteVariable::numerical
"
Parameters
----------
indice : int
	an index

Returns
-------
float
	the numerical representation of the indice-th value
"

%feature("docstring") gum::DiscreteVariable::toDiscretizedVar
"
Returns
-------
pyAgrum.DiscretizedVariable
	the discretized variable

Raises
------
gum.RuntimeError 
	If the variable is not a DiscretizedVariable
"

%feature("docstring") gum::DiscreteVariable::toLabelizedVar
"
Returns
-------
pyAgrum.LabelizedVariable
	the labelized variable

Raises
------
gum.RuntimeError
	If the variable is not a LabelizedVariable
"

%feature("docstring") gum::DiscreteVariable::toRangeVar
"
Returns
-------
pyAgrum.RangeVariable
	the range variable

Raises
------
gum.RuntimeError
	If the variable is not a RangeVariable
"

%feature("docstring") gum::DiscreteVariable::toStringWithDescription
"
Returns
-------
str
	a description of the variable
"

%feature("docstring") gum::DiscreteVariable::varType
"
returns the type of variable

Returns
-------
int :
	the type of the variable, 0: DiscretizedVariable, 1: LabelizedVariable, 2: RangeVariable
"
