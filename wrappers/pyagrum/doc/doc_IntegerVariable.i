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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/






%feature("docstring") gum::IntegerVariable
"
IntegerVariable is a discrete random variable with a customizable sequence of int.

IntegerVariable(aName, aDesc='', values=None) -> IntegerVariable
    Parameters:
        - **aName** (str) -- the name of the variable
        - **aDesc** (str) -- the (optional) description of the variable
        - **values** (List[int]) -- the values to create

IntegerVariable(aIDRV) -> IntegerVariable
    Parameters:
        - **aIDRV** (*pyagrum.IntegerVariable*) -- The pyagrum.IntegerVariable that will be copied

Examples
--------
>>> import pyagrum as gum
>>> # creating a variable with 3 values : 1,34,142
>>> va=gum.IntegerVariable('a','a integer variable',[1,34,142])
>>> print(va)
a:Integer(<1,34,142>)
>>> va.addValue(25)
(pyagrum.IntegerVariable@000001E4F5D07490) a:Integer(<1,25,34,142>)
>>> va.changeValue(34,43)
>>> print(va)
a:Integer(<1,25,43,142>)
>>> vb=gum.IntegerVariable('b','b').addValue(34).addValue(142).addValue(1)
>>> print(vb)
b:Integer(<1,34,142>)
>>> vb.labels()
('1', '34', '142')
"

%feature("docstring") gum::IntegerVariable::integerDomain
"
Returns
-------
Tuple[int]
    the tuple of integer values that form the domain of this variable
"


%feature("docstring") gum::IntegerVariable::changeValue
"
Parameters
----------
old_value : int
    the value to be changed

new_value : int
    the new value
"

%feature("docstring") gum::IntegerVariable::eraseValue
"
Parameters
----------
value: int
    the value to erase. If the value is not in the domain, the function does nothing (no exception raised)
"

%feature("docstring") gum::IntegerVariable::eraseValues
"
Remove all the domain.
"


%feature("docstring") gum::IntegerVariable::isValue
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
