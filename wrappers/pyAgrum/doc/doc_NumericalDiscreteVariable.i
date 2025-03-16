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




%feature("docstring") gum::NumericalDiscreteVariable
"
NumericalDiscreteVariable is a discrete random variable with a customizable sequence of float.

NumericalDiscreteVariable(aName, aDesc='', values=None) -> NumericalDiscreteVariable
    Parameters:
        - **aName** (str) -- the name of the variable
        - **aDesc** (str) -- the (optional) description of the variable
        - **values** (List[float]) -- the values to create equivalent to *fast* syntax `{v1|v2|v3|...|vn}`

NumericalDiscreteVariable(aName, aDesc='', first, last, nbr) -> NumericalDiscreteVariable
    Parameters:
        - **aName** (str) -- the name of the variable
        - **aDesc** (str) -- the (optional) description of the variable
        - **first** (float) -- specify a list of floats from `first` to `last` in `nbr` steps.
        - **last** (float) --
        - **nbr** (int)  -- equivalent to fast syntax `{first:last:nbr}`

NumericalDiscreteVariable(aNDRV) -> NumericalDiscreteVariable
    Parameters:
        - **aNDRV** (*pyAgrum.NumericalDiscreteVariable*) -- The pyAgrum.NumericalDiscreteVariable that will be copied

Examples
--------
>>> import pyAgrum as gum
>>> # creating a variable with 3 values : 1.5,3.14,1.42
>>> va=gum.NumericalDiscreteVariable('a','a numerica variable',[1.5,3.14,1.42])
>>> print(va)
a:NumericalDiscrete({1.42|1.5|3.14})
>>> va.addValue(2.01)
(pyAgrum.NumericalDiscreteVariable@0x55ea157b8d60) a:NumericalDiscrete({1.42|1.5|2.01|3.14})
>>> va.changeValue(3.14,3.1415)
>>> print(va)
a:NumericalDiscrete({1.42|1.5|2.01|3.1415})
>>> vb=gum.NumericalDiscreteVariable('b','b').addValue(3.14).addValue(1.42).addValue(1.5)
>>> print(vb)
b:NumericalDiscrete({1.42|1.5|3.14})
>>> vb.labels()
('1.42', '1.5', '3.14')
>>>> vc=gum.NumericalDiscreteVariable('c','c',1.2,3.8,5)
>>> print(vc)
c:NumericalDiscrete({1.2|1.85|2.5|3.15|3.8})
"

%feature("docstring") gum::NumericalDiscreteVariable::numericalDomain
"
Returns
-------
Tuple[float]
    the tuple of float values that form the domain of this variable
"


%feature("docstring") gum::NumericalDiscreteVariable::changeValue
"
Parameters
----------
old_value : int
    the value to be changed

new_value : int
    the new value
"

%feature("docstring") gum::NumericalDiscreteVariable::eraseValue
"
Parameters
----------
value: int
    the value to erase. If the value is not in the domain, the function does nothing (no exception raised)
"

%feature("docstring") gum::NumericalDiscreteVariable::eraseValues
"
Remove all the domain.
"


%feature("docstring") gum::NumericalDiscreteVariable::isValue
"
Parameters
----------
value: int
    the value to look at.

Returns
-------
bool
    True if the value is in the domain.
"
