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




%feature("docstring") gum::RangeVariable
"
RangeVariable represents a variable with a range of integers as domain.

RangeVariable(aName, aDesc,minVal, maxVal) -> RangeVariable
    Parameters:
        - **aName** (*str*) -- the name of the variable
        - **aDesc** (*str*) -- the description of the variable
        - **minVal** (int) -- the minimal integer of the interval
        - **maxVal** (int) -- the maximal integer of the interval

RangeVariable(aName, aDesc='') -> RangeVariable
    Parameters:
        - **aName** (*str*) -- the name of the variable
        - **aDesc** (*str*) -- the description of the variable

    By default ``minVal=0`` and ``maxVal=1``

RangeVariable(aRV) -> RangeVariable
    Parameters:
        - **aDV** (*RangeVariable*) -- the pyAgrum.RangeVariable that will be copied

Examples
--------
>>> import pyAgrum as gum
>>> vI=gum.RangeVariable('I','I in [4,10]',4,10)
>>> print(vI)
I:Range([4,10])
>>> vI.maxVal()
10
>>> vI.belongs(1)
False
>>> # where is the value 5 ?
>>> vI.index('5')
1
>>> vI.labels()
('4', '5', '6', '7', '8', '9', '10')
"

%feature("docstring") gum::RangeVariable::clone
"
Returns
-------
pyAgrum.RangeVariable
	a copy of the RangeVariable
"

%feature("docstring") gum::RangeVariable::belongs
"
Parameters
----------
val : long
  the value to be tested

Returns
-------
bool:
  True if the value in parameters belongs to the variable's interval.
"

%feature("docstring") gum::RangeVariable::minVal
"
Returns
-------
long :
  the lower bound of the variable
"

%feature("docstring") gum::RangeVariable::maxVal
"
Returns
-------
long :
  the upper bound of the variable.
"

%feature("docstring") gum::RangeVariable::setMaxVal
"
Set a new value of the upper bound

Parameters
----------
maxVal : long
  The new value of the upper bound

Warnings
--------
An error should be raised if the value is lower than the lower bound.
"

%feature("docstring") gum::RangeVariable::setMinVal
"
Set a new value of the lower bound

Parameters
----------
minVal : long
  The new value of the lower bound

Warnings
--------
An error should be raised if the value is higher than the upper bound.
"

%feature("docstring") gum::RangeVariable::index
"
Parameters
----------
arg2 : str
  a label

Returns
-------
int
  the indice of the label
"

%feature("docstring") gum::RangeVariable::label
"
Parameters
----------
indice : int
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
