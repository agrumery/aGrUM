/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/




%feature("docstring") gum::Tensor
"
Class representing a tensor.

Tensor() -> Tensor
    default constructor

Tensor(src) -> Tensor
    Parameters:
        - **src** (*pyagrum.Tensor*) -- the Tensor to copy
"

%feature("docstring") gum::Tensor::KL
"
Check the compatibility and compute the Kullback-Leibler divergence between the tensor and.

Parameters
----------
p : pyagrum.Tensor
  the tensor from which we want to calculate the divergence.

Returns
-------
float
  The value of the divergence

Raises
------
  pyagrum.InvalidArgument
    If p is not compatible with the tensor (dimension, variables)
  pyagrum.FatalError
    If a zero is found in p or the tensor and not in the other.
"

%feature("docstring") gum::Tensor::abs
"
Apply abs on every element of the container

Returns
-------
pyagrum.Tensor
    a reference to the modified tensor.
"

%feature("docstring") gum::Tensor::add
"
Add a discrete variable to the tensor.

Parameters
----------
v : pyagrum.DiscreteVariable
  the var to be added

Raises
------
DuplicateElement
  If the variable is already in this Tensor.
InvalidArgument
  If the variable is empty.

Returns
-------
pyagrum.Tensor
    a reference to the modified tensor.
"

%feature("docstring") gum::Tensor::contains
"
Parameters
----------
v : pyagrum.Tensor
    a DiscreteVariable.

Returns
-------
bool
    True if the var is in the tensor
"

%feature("docstring") gum::Tensor::empty
"
Returns
-------
bool
    Returns true if no variable is in the tensor.
"

%feature("docstring") gum::Tensor::entropy
"
Returns
-------
float
  the entropy of the tensor
"

%feature("docstring") gum::Tensor::extract
"
create a new Tensor extracted from self given a partial instantiation.

Parameters
----------
inst : pyagrum.instantiation
  a partial instantiation
dict : Dict[str,str|int]
  a dictionnary containing values for some discrete variables.

Warning
--------
    if the dictionnary contains a key that is not the name of a variable in the `pyagrum.Tensor`,
    this key is just not used without notification. Then `pyagrum.Tensor.extract` concerns
    only the variables that  both are in the Tensor and in the dictionnary.

Returns
-------
pyagrum.Tensor
  the new Tensor
"
%feature("docstring") gum::Tensor::fill
"
Automatically fills the tensor with v.

Parameters
----------
v : number or list or pyagrum.Tensor the number of parameters of the Tensor
    a value or a list/pyagrum.Tensor containing the values to fill the Tensor with.

Warning
-------
    if v is a list, the size of the list must be the
    if v is a pyagrum.Tensor. It must to contain variables with exactly the same names and labels but not necessarily the same variables.

Returns
-------
pyagrum.Tensor
      a reference to the modified potentia

Raises
------
pyagrum.SizeError
  If v size's does not matches the domain size.
"

%feature("docstring") gum::Tensor::fillWith
"
Automatically fills the tensor with v.

Parameters
----------
v : number or list of values or pyagrum.Tensor
    a value or a list/pyagrum.Tensor containing the values to fill the Tensor with.

mapping : list|tuple|dict

Warning
-------
    - if `v` is a list, the size of the list must be the size of the tensor

    - if `v` is a ref:pyagrum.Tensor, it must contain variables with exactly the same names and labels but not necessarily the same variables. If

    - If the second argument `mapping` is given, `mapping` explains how to map the variables of the tensor source to the variables of the tensor destination.

    - If `mapping` is a sequence, the order follows the same order as `destination.names`. If `mapping` is a dict, the keys are the names in the destination and the values are the names in the source.

Returns
-------
pyagrum.Tensor
      a reference to the modified potentia

Raises
------
pyagrum.SizeError
  If v size's does not matches the domain size.
pyagrum.ArgumentError
  If anything wrong with the arguments.
"

%feature("docstring") gum::Tensor::populate
"
Raises:
--------
pyagrum.SizeError
  if v size's does not matches the domain size.
"

%feature("docstring") gum::Tensor::get
"
Parameters
----------
i : pyagrum.Instantiation
  an Instantiation

Returns
-------
float
  the value in the Tensor at the position given by the instantiation
"

%feature("docstring") gum::Tensor::isNonZeroMap
"
Returns
-------
pyagrum.Tensor
  a boolean-like tensor using the predicate `isNonZero`.
"

%feature("docstring") gum::Tensor::maxIn
"
Projection using `max` as operation.

Parameters
----------
varnames : set
  the set of vars to keep

Returns
-------
pyagrum.Tensor
  the projected Tensor
"

%feature("docstring") gum::Tensor::maxOut
"
Projection using `max` as operation.

Parameters
----------
varnames : set
  the set of vars to eliminate

Returns
-------
pyagrum.Tensor
  the projected Tensor

Raises
------
pyagrum.InvalidArgument
  If varnames contains only one variable that does not exist in the Tensor
"

%feature("docstring") gum::Tensor::minIn
"
Projection using `min` as operation.

Parameters
----------
varnames : set
  the set of vars to keep

Returns
-------
pyagrum.Tensor
  the projected Tensor
"

%feature("docstring") gum::Tensor::minOut
"
Projection using `min` as operation.

Parameters
----------
varnames : set
  the set of vars to eliminate

Returns
-------
pyagrum.Tensor
  the projected Tensor

Warnings
--------
InvalidArgument raised if varnames contains only one variable that does not exist in the Tensor
"

%feature("docstring") gum::Tensor::prodIn
"
Projection using multiplication as operation.

Parameters
----------
varnames : set
  the set of vars to keep

Returns
-------
pyagrum.Tensor
  the projected Tensor
"

%feature("docstring") gum::Tensor::prodOut
"
Projection using multiplication as operation.

Parameters
----------
varnames : set
  the set of vars to eliminate

Returns
-------
pyagrum.Tensor
  the projected Tensor

Raises
------
pyagrum.InvalidArgument
  If varnames contains only one variable that does not exist in the Tensor
"

%feature("docstring") gum::Tensor::sumIn
"
Projection using sum as operation.

Parameters
----------
varnames : set
  the set of vars to keep

Returns
-------
pyagrum.Tensor
  the projected Tensor
"

%feature("docstring") gum::Tensor::sumOut
"
Projection using sum as operation.

Parameters
----------
varnames : set
  the set of vars to eliminate

Returns
-------
pyagrum.Tensor
  the projected Tensor

Raises
------
pyagrum.InvalidArgument
  If varnames contains only one variable that does not exist in the Tensor
"

%feature("docstring") gum::Tensor::argmax
"
Returns
-------
Tuple[Dict[str,int],float]
  the list of positions of the max and the max of all elements in the Tensor
"

%feature("docstring") gum::Tensor::max
"
Returns
-------
float
  the maximum of all elements in the Tensor
"

%feature("docstring") gum::Tensor::maxNonOne
"
Returns
-------
float
  the maximum of non one elements in the Tensor

Raises
------
pyagrum.NotFound
  If all value == 1.0
"

%feature("docstring") gum::Tensor::argmin
"
Returns
-------
Tuple[Dict[str,int],float]
  the list of positions of the min and the min of all elements in the Tensor
"

%feature("docstring") gum::Tensor::min
"
Returns
-------
float
  the min of all elements in the Tensor
"

%feature("docstring") gum::Tensor::minNonZero
"
Returns
-------
float
  the min of non zero elements in the Tensor

Raises
------
pyagrum.NotFound
  If all value == 0.0
"

%feature("docstring") gum::Tensor::nbrDim
"
Returns
-------
int
  the number of vars in the multidimensional container.
"

%feature("docstring") gum::Tensor::newFactory
"
Erase the Tensor content and create a new empty one.

Returns
-------
pyagrum.Tensor
 a reference to the new Tensor
"

%feature("docstring") gum::Tensor::normalize
"
Normalize the Tensor (do nothing if sum is 0)

Returns
-------
pyagrum.Tensor
  a reference to the normalized Tensor
"

%feature("docstring") gum::Tensor::normalizeAsCPT
"
Normalize the Tensor as a CPT

Returns
-------
pyagrum.Tensor
  a reference to the normalized Tensor

Raises
------
pyagrum.FatalError
  If some distribution sums to 0
"

%feature("docstring") gum::Tensor::pos
"
Parameters
----------
v : pyagrum.DiscreteVariable
    The variable for which the index is returned.

Returns
-------
    Returns the index of a variable.

Raises
------
pyagrum.NotFound
  If v is not in this multidimensional matrix.
"

%feature("docstring") gum::Tensor::product
"
Returns
-------
float
  the product of all elements in the Tensor
"

%feature("docstring") gum::Tensor::putFirst
"
Parameters
----------
v : pyagrum.DiscreteVariable
    The variable for which the index should be 0.

Returns
-------
pyagrum.Tensor
  a reference to the modified tensor

Raises
------
pyagrum.InvalidArgument
  If the var is not in the tensor
"

%feature("docstring") gum::Tensor::remove
"
Parameters
----------
v : pyagrum.DiscreteVariable
    The variable to be removed

Returns
-------
pyagrum.Tensor
  a reference to the modified tensor

Warnings
--------
IndexError raised if the var is not in the tensor
"

%feature("docstring") gum::Tensor::reorganize
"
Create a new Tensor with another order.

Returns
-------
varnames : list
  a list of the var names in the new order

Returns
-------
pyagrum.Tensor
  a reference to the modified tensor
"

%feature("docstring") gum::Tensor::scale
"
Create a new tensor multiplied by v.

Parameters
----------
v : float
  a multiplier

Returns
-------
  a reference to the modified tensor
"

%feature("docstring") gum::Tensor::set
"
Change the value pointed by i

Parameters
----------
i : pyagrum.Instantiation
  The Instantiation to be changed
value : float
  The new value of the Instantiation
"

%feature("docstring") gum::Tensor::sq
"
Square all the values in the Tensor
"

%feature("docstring") gum::Tensor::log2
"
log2 all the values in the Tensor

Warning
-------
When the Tensor contains 0 or negative values, no exception are raised but `-inf` or `nan` values are assigned.
"

%feature("docstring") gum::Tensor::sum
"
Returns
-------
float :
  the sum of all elements in the Tensor
"

%feature("docstring") gum::Tensor::translate
"
Create a new tensor added with v.

Parameters
----------
v : float
  The value to be added

Returns
-------
  a reference to the modified tensor
"

%feature("docstring") gum::Tensor::variable
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
pyagrum.NotFound
  If i does not reference a variable in this multidimensional matrix.
"


%feature("docstring") gum::Tensor::draw
"
draw a value using the tensor as a probability table.

Returns
-------
int
  the index of the drawn value
"

%feature("docstring") gum::Tensor::expectedValue
"
Calculate the mathematical expected value of a (joint) random variable using the given function as an argument.

Parameters
----------
func : function(Dict[str,int])->float
    A function that takes a single argument, representing the value of a python representation of a `gum.Instantiation` (as a dictionary), and returns a float.

Warnings
--------
The `pyagrum.Tensor` is assumed to contain a joint distribution.

Examples
--------
>>> def log2cptA(x):
...   return -math.log2(bn.cpt('A')[x])
>>> entropy_of_A=bn.cpt('A').expectedValue(log2cptA) # OK it A has no parents.

Returns
-------
float
    The mathematical expected value of the random variable calculated using the given function as an argument.
"


%feature("docstring") gum::Tensor::memoryFootprint
"
get the size (in byte) of the Tensor representation in memory

Returns
-------
int
  the size in byte of the representation of the Tensor in memory.
"
