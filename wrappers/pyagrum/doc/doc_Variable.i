/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/







%feature("docstring") gum::fastVariable
"
Use *fast* syntax to add a variable in the BayesNet.

Raises
------
gum.NotAllowed

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
>>> print(gum.fastVariable('A{1.2:5.2:5}}'))
A:NumericalDiscrete({1.2|2.2|3.2|4.2|5.2})
>>> print(gum.fastVariable('A{1|3|9}'))
A:Integer({1|3|9})
>>> print(gum.fastVariable('A[4,6]'))
A:Range([4,6])
>>> print(gum.fastVariable('A[5]'))
A:Range([0,4])
>>> print(gum.fastVariable('A[4,6,10]'))
A:Discretized(<[4;6[,[6;10]>)
>>> print(gum.fastVariable('A[1:6:5]'))
A:Discretized(<[1;2[,[2;3[,[3;4[,[4;5[,[5;6]>)


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
pyagrum.DiscreteVariable
	a pointer on a new copy of this
"
