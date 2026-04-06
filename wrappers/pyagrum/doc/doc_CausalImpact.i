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


%feature("docstring") gum::CausalImpact
"
Represents the result of a causal identification query P(on | do(doing), knowing).

CausalImpact encodes the identified causal formula (when the effect is
identifiable) as an expression tree that can be evaluated numerically.
It also records the identification method used and an explanation string.

The identification procedure tries the following strategies in order:
d-separation (no effect), backdoor adjustment, frontdoor adjustment, and
general do-calculus (ID algorithm).

Notes
-----
You may prefer to use the high-level function :func:`pyagrum.causal.causalImpact`
instead of constructing a CausalImpact object directly.

CausalImpact(cm, *, on, doing, knowing=None) -> CausalImpact
    Parameters:
        - **cm** (*pyagrum.causal.CausalModel*) -- the causal model.
        - **on** (*str or set of str*) -- target variable(s) of the query.
          A single string is automatically converted to a one-element set.
          Keyword-only.
        - **doing** (*str or set of str*) -- intervened variable(s)
          (the do-operator applies to these). A single string is automatically
          converted to a one-element set. Keyword-only.
        - **knowing** (*str or set of str, optional*) -- observed variable(s)
          to condition on. A single string is automatically converted to a
          one-element set. Default is empty. Keyword-only.

Examples
--------
>>> import pyagrum as gum
>>> bn = gum.BayesNet.fastPrototype('X->Y->Z')
>>> cm = gum.CausalModel(bn)
>>> formula, tensor, expl = gum.causalImpact(cm, on='Z', doing='X')
>>> print(expl)
"

%feature("docstring") gum::CausalImpact::toDict
"
Return the identified causal formula as a JSON-serialisable dictionary.

The dictionary mirrors the AST node hierarchy. Each node is a dict with
an ``\"op\"`` key identifying its type, plus type-specific keys:

- Binary operators (``+``, ``-``, ``*``, ``/``):
  ``{\"op\": \"+\", \"op1\": {...}, \"op2\": {...}}``
- Conditional probability ``P(vars | knowing)``:
  ``{\"op\": \"P\", \"vars\": [...], \"knowing\": [...]}``
- Joint probability ``P(vars)``:
  ``{\"op\": \"P\", \"vars\": [...]}``
- Summation / marginalisation :math:`\\sum_{\\text{var}}`:
  ``{\"op\": \"sum\", \"var\": \"...\", \"term\": {...}}``

Returns ``None`` if the effect is not identified
(i.e. :meth:`isIdentified` is False).

Returns
-------
dict or None
    The AST as a nested dict, or None if not identifiable.

Examples
--------
>>> import pyagrum as gum
>>> bn = gum.BayesNet.fastPrototype('X->Y->Z')
>>> cm = gum.CausalModel(bn)
>>> ci = gum.CausalImpact(cm, on='Z', doing='X')
>>> import json
>>> print(json.dumps(ci.toDict(), indent=2))
"

%feature("docstring") gum::CausalImpact::isIdentified
"
Return True if the causal effect was successfully identified.

If False, the effect cannot be computed from the available data given
the causal structure, and :meth:`eval` will raise an exception.

Returns
-------
bool
    True if the query is identifiable.
"

%feature("docstring") gum::CausalImpact::eval
"
Evaluate the identified causal formula and return the result as a tensor.

Returns
-------
pyagrum.Tensor
    The distribution P(on | do(doing), knowing).

Raises
------
pyagrum.OperationNotAllowed
    If the causal effect is not identified (:meth:`isIdentified` is False).
"

%feature("docstring") gum::CausalImpact::toString
"
Return a string representation of the identified causal formula.

Returns
-------
str
    Human-readable form of the formula (e.g. a sum-product expression over
    conditional probabilities).
"

%feature("docstring") gum::CausalImpact::toLatex
"
Return a LaTeX representation of the identified causal formula.

Parameters
----------
doOperatorPrefix : str, optional
    Prefix for the do-operator notation. Defaults to
    ``gum.config[\"causal\", \"latex_do_prefix\"]``.
doOperatorSuffix : str, optional
    Suffix for the do-operator notation. Defaults to
    ``gum.config[\"causal\", \"latex_do_suffix\"]``.

Returns
-------
str
    LaTeX string of the identified formula.
"

%feature("docstring") gum::CausalImpact::latexQuery
"
Return a LaTeX string for the original causal query before identification.

The query has the form P(on | do(doing), knowing).

Parameters
----------
doOperatorPrefix : str, optional
    Prefix for the do-operator notation. Default is 'do('.
doOperatorSuffix : str, optional
    Suffix for the do-operator notation. Default is ')'.

Returns
-------
str
    LaTeX string of the causal query.
"

%feature("docstring") gum::CausalImpact::cm
"
Return the causal model associated with this query.

Returns
-------
pyagrum.causal.CausalModel
    The causal model.
"

%feature("docstring") gum::CausalImpact::on
"
Return the NodeIds of the target variables.

Returns
-------
set of int
    NodeIds of the variables in the on-set.
"

%feature("docstring") gum::CausalImpact::doing
"
Return the NodeIds of the intervened variables.

Returns
-------
set of int
    NodeIds of the variables in the doing-set (do-operator).
"

%feature("docstring") gum::CausalImpact::knowing
"
Return the NodeIds of the observed variables.

Returns
-------
set of int
    NodeIds of the variables in the knowing-set.
"

%feature("docstring") gum::CausalImpact::explanation
"
Return a human-readable explanation of the identification result.

Describes the method used (e.g. 'd-separation', 'backdoor adjustment',
'frontdoor adjustment', 'do-calculus (ID)') or explains why identification
failed.

Returns
-------
str
    Explanation of the identification outcome.
"

%feature("docstring") gum::CausalImpact::onNames
"
Return the names of the target variables.

Returns
-------
tuple of str
    Variable names in the on-set.
"

%feature("docstring") gum::CausalImpact::doingNames
"
Return the names of the intervened variables.

Returns
-------
tuple of str
    Variable names in the doing-set.
"

%feature("docstring") gum::CausalImpact::knowingNames
"
Return the names of the observed variables.

Returns
-------
tuple of str
    Variable names in the knowing-set.
"
