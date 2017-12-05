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