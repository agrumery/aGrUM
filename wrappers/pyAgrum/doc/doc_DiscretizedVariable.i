%feature("docstring") gum::DiscretizedVariable
"
DiscretizedVariable is a discrete random variable with a set of ``ticks`` defining intervalls.

DiscretizedVariable(aName, aDesc='') -> DiscretizedVariable`
    Parameters:
        * **aName** (*str*) -- the name of the variable
        * **aDesc** (*str*) -- the (optional) description of the variable

DiscretizedVariable(aDDRV) -> DiscretizedVariable
    Parameters:
        * **aDDRV** (*pyAgrum.DiscretizedVariable*) -- the pyAgrum.DiscretizedVariable that will be copied

Examples
--------
>>> import pyAgrum as gum
>>> vX=gum.DiscretizedVariable('X','X has been discretized').addTick(1).addTick(2).addTick(3).addTick(3.1415)
>>> print(vX)
X:Discretized(<[1;2[,[2;3[,[3;3.1415]>)
>>> vX.isTick(4)
False
>>> vX.labels()
('[1;2[', '[2;3[', '[3;3.1415]')
>>> # where is the real value 2.5 ?
>>> vX.index('2.5')
1
"

%feature("docstring") gum::DiscretizedVariable::domain
"
Returns
-------
str
    the domain of the variable as a string
"

%feature("docstring") gum::DiscretizedVariable::clone
"
Returns
-------
pyAgrum.DiscretizedVariable
	a copy of the DiscretizedVariable
"

%feature("docstring") gum::DiscretizedVariable::addTick
"
Parameters
----------
aTick : double
	the Tick to be added

Returns
-------
pyAgrum.DiscretizedVariable
	the discretized variable

Raises
------
pyAgrum.DefaultInLabel
	If the Tick is already defined
"

%feature("docstring") gum::DiscretizedVariable::eraseTicks
"
erase all the Ticks 
"

%feature("docstring") gum::DiscretizedVariable::isTick
"
Parameters
----------
aTick : double
	the Tick to be tested

Returns
-------
bool :
	True if the Tick already exists
"

%feature("docstring") gum::DiscretizedVariable::tick
"
Indicate the index of the Tick

Parameters
----------
i : int
	the index of the Tick

Returns
-------
aTick : double
	the index-th Tick

Raises
------
pyAgrum.NotFound
	If the index is greater than the number of Ticks
"

%feature("docstring") gum::DiscretizedVariable::ticks
"
Returns
-------
tuple :
	a tuple containing all the Ticks
"
