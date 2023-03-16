%feature("docstring") gum::fastVariable
"
Use *fast* syntax to add a variable in the BayesNet.

Parameters
----------
fast_description: str
  string following *fast* syntax description
default_nbrmod: int
  nbr of modality if fast_description does not indicate it.
  `default_nbrmod=1` is the way to create a variable with only one value (for instance for reward in influence diagram).

Examples
--------
>>> print(gum.fastVariable('A{On|Off|Defun}'))
A:Labelized({On|Off|Defun})
>>> print(gum.fastVariable('A{3.14|0|1.15}'))
A:NumericalDiscrete({0|1.15|3.14})
>>> print(gum.fastVariable('A{1|3|9}'))
A:Integer({1|3|9})
>>> print(gum.fastVariable('A[4,6]'))
A:Range([4,6])
>>> print(gum.fastVariable('A[5]'))
A:Range([0,4])
>>> print(gum.fastVariable('A[4,6,10]'))
A:Discretized(<[4;6[,[6;10]>)


Raises
------
gum.NotAllowed if nbrmod<2
"

%feature("docstring") gum::Variable
"
Abstract class used by DiscreteVariable.
"

%feature("docstring") gum::Variable::description
"
Returns
-------
str
	the description of the variable
"

%feature("docstring") gum::Variable::name
"
Returns
-------
str
	the name of the variable
"

%feature("docstring") gum::Variable::setDescription
"
set the description of the variable.

Parameters
----------
theValue : str
	the new description of the variable
"

%feature("docstring") gum::Variable::setName
"
sets the name of the variable.

Parameters
----------
theValue : str
	the new description of the variable
"

%feature("docstring") gum::Variable::clone
"
Copy factory

Returns
-------
pyAgrum.DiscreteVariable
	a pointer on a new copy of this
"