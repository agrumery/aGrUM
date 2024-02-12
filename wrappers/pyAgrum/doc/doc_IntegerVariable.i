%feature("docstring") gum::IntegerVariable
"
IntegerVariable is a discrete random variable with a customizable sequence of int.

IntegerVariable(aName, aDesc='', values=None) -> IntegerVariable
    Parameters:
        - **aName** (str) -- the name of the variable
        - **aDesc** (str) -- the (optional) description of the variable
        - **values** (List[int]) -- the values to create

IntegerVariable(aIDRV) -> IntegerVariable
    Parameters:
        - **aIDRV** (*pyAgrum.IntegerVariable*) -- The pyAgrum.IntegerVariable that will be copied

Examples
--------
>>> import pyAgrum as gum
>>> # creating a variable with 3 values : 1,34,142
>>> va=gum.IntegerVariable('a','a integer variable',[1,34,142])
>>> print(va)
a:Integer(<1,34,142>)
>>> va.addValue(25)
(pyAgrum.IntegerVariable@000001E4F5D07490) a:Integer(<1,25,34,142>)
>>> va.changeValue(34,43)
>>> print(va)
a:Integer(<1,25,43,142>)
>>> vb=gum.IntegerVariable('b','b').addValue(34).addValue(142).addValue(1)
>>> print(vb)
b:Integer(<1,34,142>)
>>> vb.labels()
('1', '34', '142')
"

%feature("docstring") gum::IntegerVariable::integerDomain
"
Returns
-------
Tuple[int]
    the tuple of integer values that form the domain of this variable
"


%feature("docstring") gum::IntegerVariable::changeValue
"
Parameters
----------
old_value : int
    the value to be changed

new_value : int
    the new value
"

%feature("docstring") gum::IntegerVariable::eraseValue
"
Parameters
----------
value: int
    the value to erase. If the value is not in the domain, the function does nothing (no exception raised)
"

%feature("docstring") gum::IntegerVariable::eraseValues
"
Remove all the domain.
"


%feature("docstring") gum::IntegerVariable::isValue
"
Parameters
----------
value: int
    the value to look at.

Returns
-------
bool
    True if the value is in the domain.
"
