%ignore gum::DisceteVariable::clone;
%ignore gum::DisceteVariable::label;
%ignore gum::DisceteVariable::index;

%feature("docstring") gum::RangeVariable
"
RangeVariable represents a variable with a range of integers as domain.

Available constructors:

	``RangeVariable(aName, aDesc='',minVal, maxVal) -> RangeVariable``

	``RangeVariable(aName, aDesc='',minVal) -> RangeVariable``

  ``RangeVariable(aName, aDesc='') -> RangeVariable``

  ``RangeVariable(aRV) -> RangeVariable``

Parameters
----------
aName: str
  The name of the variable
aDesc: str
  The (optional) description of the variable
minVal : int
  The minimal integer of the interval
maxVal : int
  The maximal integer of the interval
aDV: pyAgrum.RangeVariable
  Another `RangeVariable` that will be copied

Examples
--------
>>> import pyAgrum as gum
>>>
>>> vI=gum.gum.RangeVariable('I','I in [4,10]',4,10)
>>> print(vI)
>>> ## I[4-10]
>>> 
>>> vX.maxVal()
>>> ## 10
>>> 
>>> vX.belongs(1)
>>> ## False
>>> 
>>> # where is the value 5 ?
>>> vX.index('5')
>>> ## 1
>>> 
>>> vi.labels()
>>> ## ('4', '5', '6', '7', '8', '9', '10')
"

%feature("docstring") gum::RangeVariable::clone
"
Returns
-------
pyAgrum.RangeVariable
	a copy of the RangeVariable
"

%feature("docstring") gum::RangeVariable::belongs
"
Parameters
----------
val : long
  the value to be tested

Returns
-------
bool:
  True if the value in parameters belongs to the variable's interval.
"

%feature("docstring") gum::RangeVariable::minVal
"
Returns
-------
long :
  the lower bound of the variable
"

%feature("docstring") gum::RangeVariable::maxVal
"
Returns
-------
long :
  the upper bound of the variable.
"

%feature("docstring") gum::RangeVariable::setMaxVal
"
Set a new value of the upper bound

Parameters
----------
maxVal : long
  The new value of the upper bound

Warnings
--------
An error should be raised if the value is lower than the lower bound.
"

%feature("docstring") gum::RangeVariable::setMinVal
"
Set a new value of the lower bound

Parameters
----------
minVal : long
  The new value of the lower bound

Warnings
--------
An error should be raised if the value is higher than the upper bound.
"

%feature("docstring") gum::RangeVariable::index
"
Parameters
----------
arg2 : str
  a label

Returns
-------
int
  the indice of the label
"

%feature("docstring") gum::RangeVariable::label
"
Parameters
----------
indice : int
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