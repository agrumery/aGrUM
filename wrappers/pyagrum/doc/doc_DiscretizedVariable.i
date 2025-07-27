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






%feature("docstring") gum::DiscretizedVariable
"
DiscretizedVariable is a discrete random variable with a set of `ticks` defining intervals.

DiscretizedVariable(aName, aDesc ,ticks=None,is_empirical=False) -> pyagrum.DiscretizedVariable Parameters:

        - **aName** (*str*) -- the name of the variable
        - **aDesc** (*str*) -- the description of the variable
        - **ticks** (*list[double]*) -- the list of ticks to add
        - **is_empirical** (*bool*) -- if False, raise an error if a value is out of bound.


DiscretizedVariable(aDDRV) -> DiscretizedVariable
    Parameters:

        - **aDDRV** (*pyagrum.DiscretizedVariable*) -- the pyagrum.DiscretizedVariable that will be copied

Examples
--------
>>> import pyagrum as gum
>>> vX=gum.DiscretizedVariable('X','X has been discretized').addTick(1).addTick(2).addTick(3).addTick(3.1415)
>>> print(vX)
X:Discretized(<[1;2[,[2;3[,[3;3.1415]>)
>>> vX.isTick(4)
False
>>> vX.labels()
('[1;2[', '[2;3[', '[3;3.1415]')
>>> # where is the real value 2.5 ?
>>> vX.index('2.5')
1
"

%feature("docstring") gum::DiscretizedVariable::domain
"
Returns
-------
str
    the domain of the variable as a string
"

%feature("docstring") gum::DiscretizedVariable::clone
"
Returns
-------
pyagrum.DiscretizedVariable
	a copy of the DiscretizedVariable
"

%feature("docstring") gum::DiscretizedVariable::addTick
"
Parameters
----------
aTick : float
	the Tick to be added

Returns
-------
pyagrum.DiscretizedVariable
	the discretized variable

Raises
------
pyagrum.DefaultInLabel
	If the Tick is already defined
"

%feature("docstring") gum::DiscretizedVariable::eraseTicks
"
erase all the Ticks
"

%feature("docstring") gum::DiscretizedVariable::isTick
"
Parameters
----------
aTick : float
	the Tick to be tested

Returns
-------
bool :
	True if the Tick already exists
"

%feature("docstring") gum::DiscretizedVariable::tick
"
Indicate the index of the Tick

Parameters
----------
i : int
	the index of the Tick

Returns
-------
aTick : float
	the i-th Tick

Raises
------
pyagrum.NotFound
	If the index is greater than the number of Ticks
"

%feature("docstring") gum::DiscretizedVariable::ticks
"
Returns
-------
tuple :
	a tuple containing all the Ticks
"


%feature("docstring") gum::DiscretizedVariable::draw
"
Allow to draw a value in the i-th interval of the discretized variable.1

Parameters
----------
i : int
	the index of the interval to draw

Returns
-------
float :
	the value randomly drawn in the i-th interval
"
