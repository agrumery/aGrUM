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


%feature("docstring") gum::Counterfactual
"
Computes a counterfactual distribution in the twin network of Balke and Pearl.

A counterfactual query asks: 'Given that we observed *profile*, what would
*on* have been if *whatif* had been set to the values in *values*?'

The twin network (`pyagrum.counterfactualModel`) contains the real world and
a copy ``V*`` of the *whatif* variables and of their descendants (the
counterfactual world), both worlds sharing the other variables, in
particular the parentless ones. The counterfactual distribution is the
posterior of the copies of *on* given the *profile* (observed in the real
world) and the values of the copies of *whatif*: the abduction, action and
prediction steps of Pearl are done by a single exact inference, which keeps
the dependencies created by the profile between the shared variables.

Notes
-----
Prefer using the high-level function :func:`pyagrum.counterfactual`
instead of constructing a Counterfactual object directly.

A causal model with latent variables is refused (`pyagrum.InvalidArgument`):
their distributions, needed for the abduction, are unknown.

Counterfactual(cm, on, whatif, profile={}, values={}) -> Counterfactual
    Parameters:
        - **cm** (`pyagrum.CausalModel`) -- the causal model.
        - **on** (*set of str or set of int*) -- target variables of the
          counterfactual query.
        - **whatif** (*set of str or set of int*) -- variables whose values
          are changed in the counterfactual scenario.
        - **profile** (*dict of str → str*) -- the factual observation,
          given as ``{variable_name: value_name}``. Default is empty.
        - **values** (*dict of str → str*) -- counterfactual values for the
          *whatif* variables, given as ``{variable_name: value_name}``.
          If omitted, the full joint distribution over all *whatif* values
          is returned.
"

%feature("docstring") gum::Counterfactual::run
"
Compute the counterfactual distribution by inference in the twin network.

This method is called automatically by the constructor.
"

%feature("docstring") gum::Counterfactual::originalModel
"
Return the original causal model.

Returns
-------
pyagrum.CausalModel
    The original causal model passed to the constructor.
"

%feature("docstring") gum::Counterfactual::twinModel
"
Return the twin network of Balke and Pearl used for the computation.

It contains the variables of the original model (real world) and a copy
``V*`` of each *whatif* variable and of each of their descendants
(counterfactual world). See `pyagrum.counterfactualModel`.

Returns
-------
pyagrum.CausalModel
    The twin network.
"

// gum::Counterfactual::impact (deprecated) is documented in causal_after_templates.i,
// where it is rewritten in Python.

%feature("docstring") gum::Counterfactual::copyName
"
Return the name of the copy of a variable in the twin network.

Parameters
----------
name : str
    The name of a variable of the original model.

Returns
-------
str
    The name of its copy in the counterfactual world (``name*``).
"

%feature("docstring") gum::Counterfactual::value
"
Return the counterfactual distribution as a tensor.

Returns
-------
pyagrum.Tensor
    The distribution P(on | do(whatif)) evaluated on the twin model,
    optionally sliced by *values* if provided at construction.
"

%feature("docstring") gum::Counterfactual::on
"
Return the names of the target variables.

Returns
-------
set of str
    Variable names in the on-set.
"

%feature("docstring") gum::Counterfactual::whatif
"
Return the names of the intervened variables.

Returns
-------
set of str
    Variable names in the whatif-set.
"

%feature("docstring") gum::Counterfactual::profile
"
Return the factual evidence used for abduction.

Returns
-------
dict of str → str
    The observed profile as ``{variable_name: value_name}``.
"

%feature("docstring") gum::Counterfactual::values
"
Return the counterfactual assignments for the whatif variables.

Returns
-------
dict of str → str
    The counterfactual values as ``{variable_name: value_name}``.
    Empty dict if no specific values were requested.
"

%feature("docstring") gum::Counterfactual::toString
"
Return a string description of the counterfactual query and its result.

Returns
-------
str
    Human-readable summary of the counterfactual computation.
"
