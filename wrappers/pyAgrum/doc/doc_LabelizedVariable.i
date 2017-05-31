%feature("docstring") gum::LabelizedVariable
"
LabelizedVariable is a discrete random variable with a customizable sequence of labels.

Available constructors:

    `LabelizedVariable(aName, aDesc='', nbrLabel=2) -> LabelizedVariable`

    `LabelizedVariable(aLDRV) -> LabelizedVariable`

Parameters
----------
aName: str
  The name of the variable
aDesc: str
  The (optional) description of the variable
nbrLabel: int
  The number of labels to create. By default , the value start from '0' to 'nbrLabel-1'
aLDRV: pyAgrum.LabelizedVariable
  Another `LabelizedVariable` that will be copied

Examples
--------
>>> import pyAgrum as gum
>>> v=gum.LabelizedVariable('v')
>>> print(v)
v<0,1>
>>> w=gum.LabelizedVariable('w','',4)
>>> print(w)
w<0,1,2,3>
"
