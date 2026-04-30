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
Computes a counterfactual distribution using Pearl's twin network method.

A counterfactual query asks: 'Given that we observed *profile*, what would
*on* have been if *whatif* had been set to the values in *values*?'

The computation follows the three-step algorithm from Pearl (2018),
*The Book of Why*, chapter 8:

  1. **Abduction** -- compute the posterior distribution of parentless
     (idiosyncratic) nodes in the original BN given the observed *profile*.
  2. **Action** -- build a *twin* causal model in which the priors of those
     nodes are replaced by their posteriors from step 1, then apply
     ``do(whatif)`` on the twin.
  3. **Prediction** -- evaluate the causal impact of the intervention on
     the twin model to obtain the counterfactual distribution of *on*.

Notes
-----
Prefer using the high-level function :func:`pyagrum.counterfactual`
instead of constructing a Counterfactual object directly.

Counterfactual(cm, on, whatif, profile={}, values={}) -> Counterfactual
    Parameters:
        - **cm** (*pyagrum.CausalModel*) -- the causal model.
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

Examples
--------
>>> import pyagrum as gum
>>> bn = gum.BayesNet.fastPrototype('X->Y->Z')
>>> cm = gum.CausalModel(bn)
>>> t = gum.counterfactual(cm, on='Z', whatif='X',
...                        profile={'Y': 'True'}, values={'X': 'False'})
"

%feature("docstring") gum::Counterfactual::counterFactualModel
"
Build the twin causal model from the original model and a factual profile.

This static method implements steps 1-2 of the three-step counterfactual
algorithm: it computes the posterior of parentless (idiosyncratic) nodes
in the observational BN given the profile evidence, then replaces their
prior distributions in a copy of the model with those posteriors.

Parameters
----------
cm : pyagrum.CausalModel
    The original causal model.
profile : dict of str → str
    The factual observation as ``{variable_name: value_name}``.
whatif : set of str
    Intervened variables in the counterfactual scenario. These are excluded
    from the set of idiosyncratic nodes that get updated.

Returns
-------
pyagrum.CausalModel
    The twin causal model ready for the prediction step.

See Also
--------
pyagrum.counterfactualModel : high-level function wrapper.
"

%feature("docstring") gum::Counterfactual::run
"
Execute the counterfactual computation (steps 2 and 3).

This method is called automatically by the constructor. Call it explicitly
only if you need to re-run after modifying the object's state.
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
Return the twin causal model built during the abduction step.

The twin model has the same structure as the original but with updated
priors for parentless nodes (based on the observed profile).

Returns
-------
pyagrum.CausalModel
    The twin model.
"

%feature("docstring") gum::Counterfactual::impact
"
Return the CausalImpact used to compute the counterfactual distribution.

The formula is evaluated on the twin model.

Returns
-------
pyagrum.CausalImpact
    The identified causal impact on the twin model.
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
