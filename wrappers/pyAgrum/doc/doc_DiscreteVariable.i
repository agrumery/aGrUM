/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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

%feature("docstring") gum::DiscreteVariable
"
DiscreteVariable is the (abstract) base class for discrete random variables.
"

%feature("docstring") gum::DiscreteVariable::clone
"
Returns
-------
  pyAgrum.DiscreteVariable
	a copy of the DiscreteVariable
"

%feature("docstring") gum::DiscreteVariable::domain
"
Returns
-------
str
	the domain of the variable
"

%feature("docstring") gum::DiscreteVariable::stype
"
Returns
-------
str
	a description of its type
"

%feature("docstring") gum::DiscreteVariable::domainSize
"
Returns
-------
int
	the number of modalities in the variable domain
"

%feature("docstring") gum::DiscreteVariable::empty
"
Returns
-------
bool
	True if the domain size < 2
"

%feature("docstring") gum::DiscreteVariable::index
"
Parameters
----------
label : str
	a label

Returns
-------
int
	the indice of the label
"

%feature("docstring") gum::DiscreteVariable::label
"
Parameters
----------
i : int
	the index of the label we wish to return

Returns
-------
str
	the indice-th label

Raises
------
pyAgrum.OutOfBounds
	If the variable does not contain the label
"

%feature("docstring") gum::DiscreteVariable::labels
"
Returns
-------
tuple
	a tuple containing the labels
"

%feature("docstring") gum::DiscreteVariable::numerical
"
Parameters
----------
indice : int
	an index

Returns
-------
float
	the numerical representation of the indice-th value
"

%feature("docstring") gum::DiscreteVariable::toDiscretizedVar
"
Returns
-------
pyAgrum.DiscretizedVariable
	the discretized variable

Raises
------
pyAgrum.RuntimeError
	If the variable is not a DiscretizedVariable
"

%feature("docstring") gum::DiscreteVariable::toLabelizedVar
"
Returns
-------
pyAgrum.LabelizedVariable
	the labelized variable

Raises
------
pyAgrum.RuntimeError
	If the variable is not a LabelizedVariable
"

%feature("docstring") gum::DiscreteVariable::toRangeVar
"
Returns
-------
pyAgrum.RangeVariable
	the range variable

Raises
------
pyAgrum.RuntimeError
	If the variable is not a RangeVariable
"

%feature("docstring") gum::DiscreteVariable::toStringWithDescription
"
Returns
-------
str
	a description of the variable
"

%feature("docstring") gum::DiscreteVariable::varType
"
returns the type of variable

Returns
-------
int :
	the type of the variable.

	0: DiscretizedVariable, 1: LabelizedVariable, 2: IntegerVariable, 3: RangeVariable, 4:
"

%feature("docstring") gum::DiscreteVariable::asLabelizedVar
"
Tries to cast the variable as a `gum.LabelizedVar`.

Raises
------
gum.OperationNotAllowed if this is not possible

Returns
-------
gum.LabelizedVar
  the variable as a gum.LabelizedVar
"

%feature("docstring") gum::DiscreteVariable::asRangeVar
"
Tries to cast the variable as a `gum.RangeVar`.

Raises
------
gum.OperationNotAllowed if this is not possible

Returns
-------
gum.RangeVar
  the variable as a gum.RangeVar
"

%feature("docstring") gum::DiscreteVariable::asIntegerVar
"
Tries to cast the variable as a `gum.IntegerVar`.

Raises
------
gum.OperationNotAllowed if this is not possible

Returns
-------
gum.IntegerVar
  the variable as a gum.IntegerVar
"

%feature("docstring") gum::DiscreteVariable::asNumericalDiscreteVar
"
Tries to cast the variable as a `gum.NumericalDiscreteVar`.

Raises
------
gum.OperationNotAllowed if this is not possible

Returns
-------
gum.NumericalDiscreteVar
  the variable as a gum.NumericalDiscreteVar
"

%feature("docstring") gum::DiscreteVariable::asDiscretizedVar
"
Tries to cast the variable as a `gum.DiscretizedVar`.

Raises
------
gum.OperationNotAllowed if this is not possible

Returns
-------
gum.DiscretizedVar
  the variable as a gum.DiscretizedVar
"
