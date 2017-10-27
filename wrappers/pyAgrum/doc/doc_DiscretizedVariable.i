%ignore gum::DisceteVariable::clone;

%feature("docstring") gum::DiscretizedVariable
"
DiscretizedVariable is a discrete random variable with a set of ``ticks`` defining intervalls.

Available constructors:

    ``DiscretizedVariable(aName, aDesc='') -> DiscretizedVariable``

    ``DiscretizedVariable(aDDRV) -> DiscretizedVariable``

Parameters
----------
aName: str
  The name of the variable
aDesc: str
  The (optional) description of the variable
aDDRV: pyAgrum.DiscretizedVariable
  Another `DiscretizedVariable` that will be copied

Examples
--------
>>> import pyAgrum as gum
>>>
>>> vX=gum.DiscretizedVariable('X','X has been discretized')
>>> vX.addTick(1).addTick(2).addTick(3).addTick(3.1415) #doctest: +ELLIPSIS
>>> ## <pyAgrum.pyAgrum.DiscretizedVariable;...>
>>> print(vX)
>>> ## X<[1;2[,[2;3[,[3;3.1415]>
>>>
>>> vX.isTick(4)
>>> ## False
>>>
>>> vX.labels()
>>> ## ('[1;2[', '[2;3[', '[3;3.1415]')
>>>
>>> # where is the real value 2.5 ?
>>> vX.index('2.5')
>>> ## 1
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
