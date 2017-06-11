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
>>> v=gum.DiscretizedVariable('v','a descr')
>>> print(v)
v<>
>>> v.addTick(1).addTick(3.14).addTick(0.4).addTick(0.2)
>>> print(w)
a<[0.2;0.4[,[0.4;1[,[1;3.14]>
"

%feature("docstring") gum::DiscretizedVariable::domain
"
Returns
-------
str
    the domain of the variable as a string
"
