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






%feature("docstring") gum::LabelizedVariable
"
LabelizedVariable is a discrete random variable with a customizable sequence of labels.

LabelizedVariable(aName, aDesc='', nbrLabel=2) -> LabelizedVariable
    Parameters:
        - **aName** (str) -- the name of the variable
        - **aDesc** (str) -- the (optional) description of the variable
        - **nbrLabel** (int) -- the number of labels to create (2 by default)

LabelizedVariable(aName, aDesc='', labels) -> LabelizedVariable
    Parameters:
        - **aName** (str) -- the name of the variable
        - **aDesc** (str) -- the (optional) description of the variable
        - **labels** (List[str]) -- the labels to create

LabelizedVariable(aLDRV) -> LabelizedVariable
    Parameters:
        - **aLDRV** (*pyagrum.LabelizedVariable*) -- The pyagrum.LabelizedVariable that will be copied

Examples
--------
>>> import pyagrum as gum
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
pyagrum.DuplicateElement
  If the variable already contains the new label
pyagrum.OutOfBounds
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
pyagrum.LabelizedVariable
	a copy of the LabelizedVariable
"
