%ignore gum::DisceteVariable::clone;

%feature("docstring") gum::RangeVariable
"
RangeVariable represents a variable with a range of integers as domain.

Available constructors:

	``RangeVariable(aName, aDesc='',minVal, maxVal) -> RangeVariable``

	``RangeVariable(aName, aDesc='',minVal) -> DiscreRangeVariableteVariable``

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