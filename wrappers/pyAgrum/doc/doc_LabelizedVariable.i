%ignore gum::DisceteVariable::clone;

%feature("docstring") gum::LabelizedVariable
"
LabelizedVariable is a discrete random variable with a customizable sequence of labels.

Available constructors:

    ``LabelizedVariable(aName, aDesc='', nbrLabel=2) -> LabelizedVariable``

    ``LabelizedVariable(aName, aDesc='') -> LabelizedVariable``

    ``LabelizedVariable(aLDRV) -> LabelizedVariable``

Parameters
----------
aName: str
  The name of the variable
aDesc: str
  The (optional) description of the variable
nbrLabel: int
  The number of labels to create (2 by default)
aLDRV: pyAgrum.LabelizedVariable
  Another `LabelizedVariable` that will be copied

Examples
--------
>>> import pyAgrum as gum
>>>
>>> # creating a variable with 3 labels : '0', '1' and '2'
>>> va=gum.LabelizedVariable('a','a labelized variable',3)
>>> print(va)
>>> ## a<0,1,2>
>>>
>>> va.addLabel('foo')
>>> print(va)
>>> ## a<0,1,2,foo>
>>>
>>> va.chgLabel(1,'bar')
>>> print(va)
>>> a<0,bar,2,foo>
>>>
>>> vb=gum.LabelizedVariable('b','b',0).addLabel('A').addLabel('B').addLabel('C')
>>> print(vb)
>>> ## b<A,B,C>
>>> 
>>> vb.labels()
>>> ## ('A', 'B', 'C')
>>> 
>>> vb.isLabel('E')
>>> ## False
>>> 
>>> vb.label(2)
>>> ## 'B'
"


%feature("docstring") gum::LabelizedVariable::domain
"
Returns
-------
str
    the domain of the variable as a string
"

%feature("docstring") gum::LabelizedVariable::clone
"
Returns
-------
pyAgrum.LabelizedVariable
	a copy of the LabelizedVariable
"