%feature("docstring") gum::Potential
"
Class representing a potential.

Available constructors:

  ``Potential() -> Potential``

  ``Potential(src) -> Potential``

Parameters
----------
src: pyAgrum.Potential
  The Potential to copy

Returns
-------
pyAgrum.Potential
  The new Potential

"

%feature("docstring") gum::Potential::KL
"
C heck the compatibility and compute the Kullback-Leibler divergence between the potential and.

Parameters
----------
p : pyAgrum.Potential
  the potential from which we want to calculate the divergence.

Returns
-------
float
  The value of the divergence

Raises
------
gum.InvalidArgument
  If p is not compatible with the potential (dimension, variables)
gum.FatalError
  If a zero is found in p or the potential and not in the other. 
"

%feature("docstring") gum::Potential::abs
"
Apply abs on every element of the container

Returns
-------
pyAgrum.Potential
    a reference to the modified potential.
"

%feature("docstring") gum::Potential::add
"
Add a discrete variable to the potential.

Parameters
----------
v : pyAgrum.DiscreteVariable
  the var to be added

Raises
------
gum.Error
  If DiscretizedVariable added with no Tick.
"

%feature("docstring") gum::Potential::contains
"
Parameters
----------
v : pyAgrum.Potential
    a DiscreteVariable.

Returns
-------
bool
    True if the var is in the potential
"

%feature("docstring") gum::Potential::empty
"
Returns
-------
bool
    Returns true if no variable is in the potential. 
"

%feature("docstring") gum::Potential::entropy
"
Returns
-------
double
  the entropy of the potential
"

%feature("docstring") gum::Potential::extract
"
create a new Potential extracted from self given a partial instantiation.

Parameters
----------
inst : pyAgrum.instantiation
  a partial instantiation
dict : dict
  a dictionnary containing discrete variables (?)

Returns
-------
pyAgrum.Potential
  the new Potential
"

%feature("docstring") gum::Potential::fill
"
Automatically fills the potential with v.

Parameters
----------
v : number or list or pyAgrum.Potential the number of parameters of the Potential
    a value or a list/pyAgrum.Potential containing the values to fill the Potential with.

Warning
-------
    if v is a list, the size of the list must be the
    if v is a pyAgrum.Potential. It must to contain variables with exactly the same names and labels but not necessarily the same variables.

Returns
-------
pyAgrum.Potential
      a reference to the modified potentia

Raises
------
gum.SizeError
  If v size's does not matches the domain size. 
"

%feature("docstring") gum::Potential::populate
"
Raises:
--------
gum.SizeError 
  if v size's does not matches the domain size.
"

%feature("docstring") gum::Potential::get
"
Parameters
----------
i : pyAgrum.Instantiation
  an Instantiation

Returns
-------
double
  the value of the instantiation
"

%feature("docstring") gum::Potential::isNonZeroMap
"
Returns
-------
pyAgrum.Potential
  a boolean-like potential using the predicate isNonZero
"

%feature("docstring") gum::Potential::margMaxIn
"
Projection using max as operation.

Parameters
----------
varnames : set
  the set of vars to keep

Returns
-------
pyAgrum.Potential
  the projected Potential
"

%feature("docstring") gum::Potential::margMaxOut
"
Projection using max as operation.

Parameters
----------
varnames : set
  the set of vars to eliminate

Returns
-------
pyAgrum.Potential
  the projected Potential

Raises
------
gum.InvalidArgument
  If varnames contains only one variable that does not exist in the Potential
"

%feature("docstring") gum::Potential::margMinIn
"
Projection using min as operation.

Parameters
----------
varnames : set
  the set of vars to keep

Returns
-------
pyAgrum.Potential
  the projected Potential
"

%feature("docstring") gum::Potential::margMinOut
"
Projection using min as operation.

Parameters
----------
varnames : set
  the set of vars to eliminate

Returns
-------
pyAgrum.Potential
  the projected Potential

Warnings
--------
InvalidArgument raised if varnames contains only one variable that does not exist in the Potential
"

%feature("docstring") gum::Potential::margProdIn
"
Projection using multiplication as operation.

Parameters
----------
varnames : set
  the set of vars to keep

Returns
-------
pyAgrum.Potential
  the projected Potential
"

%feature("docstring") gum::Potential::margProdOut
"
Projection using multiplication as operation.

Parameters
----------
varnames : set
  the set of vars to eliminate

Returns
-------
pyAgrum.Potential
  the projected Potential

Raises
------
gum.InvalidArgument
  If varnames contains only one variable that does not exist in the Potential
"

%feature("docstring") gum::Potential::margSumIn
"
Projection using sum as operation.

Parameters
----------
varnames : set
  the set of vars to keep

Returns
-------
pyAgrum.Potential
  the projected Potential
"

%feature("docstring") gum::Potential::margSumOut
"
Projection using sum as operation.

Parameters
----------
varnames : set
  the set of vars to eliminate

Returns
-------
pyAgrum.Potential
  the projected Potential

Raises
------
gum.InvalidArgument 
  If varnames contains only one variable that does not exist in the Potential
"

%feature("docstring") gum::Potential::max
"
Returns
-------
double
  the maximum of all elements in the Potential
"

%feature("docstring") gum::Potential::maxNonOne
"
Returns
-------
double
  the maximum of non one elements in the Potential

Raises
------
gum.NotFound
  If all value == 1.0
"

%feature("docstring") gum::Potential::min
"
Returns
-------
double
  the min of all elements in the Potential
"

%feature("docstring") gum::Potential::minNonZero
"
Returns
-------
double
  the min of non zero elements in the Potential

Raises
------
gum.NotFound
  If all value == 0.0
"

%feature("docstring") gum::Potential::nbrDim
"
Returns
-------
int
  the number of vars in the multidimensional container. 
"

%feature("docstring") gum::Potential::newFactory
"
Erase the Potential content and create a new empty one.

Returns
-------
pyAgrum.Potential
 a reference to the new Potential
"

%feature("docstring") gum::Potential::normalize
"
Normalize the Potential (do nothing if sum is 0)

Returns
-------
pyAgrum.Potential
  a reference to the normalized Potential
"

%feature("docstring") gum::Potential::normalizeAsCPT
"
Normalize the Potential as a CPT

Returns
-------
pyAgrum.Potential
  a reference to the normalized Potential

Raises
------
gum.FatalError
  If some distribution sums to 0
"

%feature("docstring") gum::Potential::pos
"
Parameters
----------
v : pyAgrum.DiscreteVariable
    The variable for which the index is returned.

Returns
-------
    Returns the index of a variable. 

Raises
------
gum.NotFound 
  If v is not in this multidimensional matrix. 
"

%feature("docstring") gum::Potential::product
"
Returns
-------
double
  the product of all elements in the Potential
"

%feature("docstring") gum::Potential::putFirst
"
Parameters
----------
v : pyAgrum.DiscreteVariable
    The variable for which the index should be 0.

Returns
-------
pyAgrum.Potential
  a reference to the modified potential

Raises
------
gum.InvalidArgument
  If the var is not in the potential 
"

%feature("docstring") gum::Potential::remove
"
Parameters
----------
v : pyAgrum.DiscreteVariable
    The variable to be removed

Returns
-------
pyAgrum.Potential
  a reference to the modified potential

Warnings
--------
IndexError raised if the var is not in the potential 
"

%feature("docstring") gum::Potential::reorganize
"
Create a new Potential with another order.

Returns
-------
varnames : list
  a list of the var names in the new order

Returns
-------
pyAgrum.Potential
  a reference to the modified potential
"

%feature("docstring") gum::Potential::scale
"
Create a new potential multiplied by v.

Parameters
----------
v : double
  a multiplier

Returns
-------
  a reference to the modified potential
"

%feature("docstring") gum::Potential::set
"
Change the value pointed by i

Parameters
----------
i : pyAgrum.Instantiation
  The Instantiation to be changed
value : double
  The new value of the Instantiation
"

%feature("docstring") gum::Potential::sq
"
Square all the values in the Potential
"

%feature("docstring") gum::Potential::sum
"
Returns
-------
double :
  the sum of all elements in the Potential
"

%feature("docstring") gum::Potential::translate
"
Create a new potential added with v.

Parameters
----------
v : double
  The value to be added

Returns
-------
  a reference to the modified potential
"

%feature("docstring") gum::Potential::variable
"
Parameters
----------
i : int 
  An index of this multidimensional matrix.

Returns
-------
  the varible at the ith index 

Raises
------
gum.NotFound
  If i does not reference a variable in this multidimensional matrix. 
"


%feature("docstring") gum::Potential::draw
"
draw a value using the potential as a probability table.

Returns
-------
int
  the index of the drawn value
"
