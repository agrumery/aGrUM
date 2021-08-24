%feature("docstring") gum::LabelizedVariable
"
LabelizedVariable is a discrete random variable with a customizable sequence of labels.

LabelizedVariable(aName, aDesc='', nbrLabel=2) -> LabelizedVariable
    Parameters:
        * **aName** (*str*) -- the name of the variable
        * **aDesc** (*str*) -- the (optional) description of the variable
        * **nbrLabel** (*int*) -- the number of labels to create (2 by default)

LabelizedVariable(aLDRV) -> LabelizedVariable
    Parameters:
        * **aLDRV** (*pyAgrum.LabelizedVariable*) -- The pyAgrum.LabelizedVariable that will be copied

Examples
--------
>>> import pyAgrum as gum
>>> # creating a variable with 3 labels : '0', '1' and '2'
>>> va=gum.LabelizedVariable('a','a labelized variable',3)
>>> print(va)
a:Labelized(<0,1,2>)
>>> va.addLabel('foo')
(gum::LabelizedVariable@0x7fc4c840dd90) a:Labelized(<0,1,2,foo>)
>>> va.changeLabel(1,'bar')
>>> print(va)
a:Labelized(<0,bar,2,foo>)
>>> vb=gum.LabelizedVariable('b','b',0).addLabel('A').addLabel('B').addLabel('C')
>>> print(vb)
b:Labelized(<A,B,C>)
>>> vb.labels()
('A', 'B', 'C')
>>> vb.isLabel('E')
False
>>> vb.label(2)
'C'
>>> vc=gum.LabelizedVariable('b','b',['one','two','three'])
>>> vc
(gum::LabelizedVariable@0x7fc4c840c130) b:Labelized(<one,two,three>)
"

%feature("docstring") gum::LabelizedVariable::changeLabel
"
Change the label at the specified index

Parameters
----------
pos : int
	the index of the label to be changed
aLabel : str
	the label to be added to the labelized variable

Raises
------
pyAgrum.DuplicatedElement
  If the variable already contains the new label
pyAgrum.OutOfBounds
  If the index is greater than the size of the variable
"

%feature("docstring") gum::LabelizedVariable::eraseLabels
"
Erase all the labels from the variable.
"

%feature("docstring") gum::LabelizedVariable::isLabel
"
Indicates whether the variable already has the label passed in argument

Parameters
----------
aLabel : str
	the label to be tested

Returns
-------
bool
	True if the label already exists
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
