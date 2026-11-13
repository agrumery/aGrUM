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

%feature("docstring") gum::fastNameNeedsQuotes
"
Tell whether the name of a variable must be quoted to be read back as is by `pyagrum.fastVariable` and by the
fast syntax of the graphical models (`pyagrum.fastBN`, `pyagrum.fastID`, `pyagrum.fastMRF`).

A name is written between single quotes (``'Visit Asia'->X``) when it begins or ends with a space, begins with a
quote, ``'*'`` or ``'$'``, contains one of ``[]{};-<>`` or a line break, ends with ``'+'``, or contains a quote at
the beginning of a word. Spaces inside a name do not need quotes there. The node names of `pyagrum.fastGraph` are
stricter: an unquoted one contains no space, and is a node id if it is made of digits.

Parameters
----------
name : str
  the name

Returns
-------
bool
  True if the name must be quoted
"

%feature("docstring") gum::quoteFastName
"
Write a name as in a fast description: between single quotes (with its quotes doubled) if
`pyagrum.fastNameNeedsQuotes`, as is otherwise.

This is what ``toFast()`` does for the name of a variable.

Warnings
--------
The files saved by pyAgrum (``.jgum``, ``.bgum``, ``.bifxml``, ``.pkl``) write the variables in fast syntax: a
quoted name such as ``'X-ray'[3]`` (or the name ``X[0]`` of a variable of a `pyagrum.ktbn.KTBN`) is read by
pyAgrum 3.3.2 or older with the quotes in the name.

Parameters
----------
name : str
  the name

Returns
-------
str
  the name, quoted if needed
"

%feature("docstring") gum::checkFastSyntax
"
Check a fast description for every fast syntax.

A description may be valid for several syntaxes: ``'A->B'`` is a directed graph, a mixed graph, a marked
graph, a Bayesian network and an influence diagram. It is valid for a syntax if the corresponding fast
function accepts it, apart from the constraints checked when the object is built (acyclicity, children of a
utility node...).

Parameters
----------
description : str
  the fast description
defaultDomain : str
  the domain of the variables without one, for the graphical models (``'[2]'`` by default)

Returns
-------
pyagrum.FastSyntaxCheck
  the verdict for every syntax

Raises
------
pyagrum.InvalidArgument
  If defaultDomain does not begin with ``'['`` or ``'{'``.
"

%feature("docstring") gum::FastSyntaxCheck
"
The verdict on a fast description for every fast syntax (see `pyagrum.checkFastSyntax`).

A FastSyntaxCheck is true if the description is valid for at least one syntax.

FastSyntaxCheck(description, defaultDomain='[2]') -> FastSyntaxCheck

Parameters
----------
description : str
  the fast description
defaultDomain : str
  the domain of the variables without one, for the graphical models
"

%feature("docstring") gum::FastSyntaxCheck::description
"
Returns
-------
str
  the checked description
"

%feature("docstring") gum::FastSyntaxCheck::isValid
"
Tell whether the description is valid for a syntax.

Parameters
----------
syntax : pyagrum.FastSyntax
  the syntax

Returns
-------
bool
  True if the description is valid for this syntax
"

%feature("docstring") gum::FastSyntaxCheck::error
"
Tell why the description is not valid for a syntax.

Parameters
----------
syntax : pyagrum.FastSyntax
  the syntax

Returns
-------
str
  the error (empty if the description is valid for this syntax)
"

%feature("docstring") gum::FastSyntaxCheck::__bool__
"
Returns
-------
bool
  True if the description is valid for at least one syntax
"

%feature("docstring") gum::FastSyntaxCheck::__repr__
"
Returns
-------
str
  the description and the syntaxes for which it is valid
"
