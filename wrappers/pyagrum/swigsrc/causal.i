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

// causalImpact, counterfactual and counterfactualModel are renamed with a leading
// underscore so that the %pythoncode wrappers below can expose them with explicit
// keyword arguments and type coercion (str→set for on/doing/knowing).
// %feature("kwargs") is NOT used here: it causes SIGSEGV on inline-constructed
// temporaries because the **kwargs dispatch can drop the refcount of arguments
// (e.g. gum.CausalModel(edex)) before the C++ call completes.
%rename(_causalImpact) gum::causalImpact;
%rename(_counterfactual) gum::counterfactual;
%rename(_counterfactualModel) gum::counterfactualModel;

// enumerateBackdoorSets and enumerateFrontdoorSets are renamed so that the
// %pythoncode wrappers in causal_after_templates.i can enforce keyword-only
// arguments starting from excluded_nodes.
%rename(_enumerateBackdoorSets)  gum::DoorCriteria::enumerateBackdoorSets;
%rename(_enumerateFrontdoorSets) gum::DoorCriteria::enumerateFrontdoorSets;

// LatentDescriptorVector and LatentDescriptorIds are C++-internal types with no
// Python representation. Ignoring them causes SWIG to suppress all overloads
// that use them (including the LatentDescriptorVector constructors of CausalModel),
// keeping the Python error messages free of internal C++ type names.
%ignore gum::LatentDescriptorIds;
%ignore gum::LatentDescriptorVector;


%pythoncode %{
def causalImpact(cm, on, doing, knowing=None, values=None):
    """
    Identify and evaluate the causal effect of do(doing) on on, optionally
    conditioning on knowing.

    The identification procedure tries in order: d-separation (no effect),
    backdoor adjustment, frontdoor adjustment, and general do-calculus
    (ID algorithm).

    Parameters
    ----------
    cm : pyagrum.causal.CausalModel
        The causal model.
    on : str or set of str
        Target variable(s) of the causal query. A single string is
        automatically converted to a one-element set.
    doing : str or set of str
        Intervened variable(s) (the do-operator applies to these). A single
        string is automatically converted to a one-element set.
    knowing : str or set of str, optional
        Observed variable(s) to condition on. Default is empty.
    values : dict of str → str, optional
        Specific values for the on/knowing variables as
        ``{variable_name: value_name}``. When provided, the returned tensor
        is sliced to those values. Default is no slicing.

    Returns
    -------
    tuple (pyagrum.causal.CausalImpact, pyagrum.Tensor or None, str)
        - The CausalImpact object encoding the identified formula.
        - The evaluated tensor P(on | do(doing), knowing), or None if the
          effect is not identifiable.
        - A string explaining the identification method used or why
          identification failed.

    Examples
    --------
    >>> import pyagrum as gum
    >>> import pyagrum.causal as csl
    >>> bn = gum.BayesNet.fastPrototype('X->Y->Z')
    >>> cm = csl.CausalModel(bn)
    >>> formula, tensor, expl = csl.causalImpact(cm, on='Z', doing='X')
    >>> print(expl)
    """
    if isinstance(on, str):
        on = {on}
    if isinstance(doing, str):
        doing = {doing}
    if isinstance(knowing, str):
        knowing = {knowing}
    lat, pot, expl = _causalImpact(cm, on, doing,
                                   knowing if knowing is not None else set(),
                                   values  if values  is not None else {})
    pot._model = cm
    return lat, (pot if lat.isIdentified() else None), expl

def counterfactual(cm, on, whatif, profile=None, values=None):
    """
    Compute a counterfactual distribution using Pearl's twin network method.

    Answers the question: 'Given that we observed *profile*, what would
    *on* have been if *whatif* had been set as specified in *values*?'

    The computation follows the three-step algorithm from Pearl (2018),
    *The Book of Why*, chapter 8: abduction (update parentless node priors
    from the profile), action (apply do(whatif) on the twin model), and
    prediction (evaluate the causal effect on the twin).

    Parameters
    ----------
    cm : pyagrum.causal.CausalModel
        The causal model.
    on : str or set of str
        Target variable(s) of the counterfactual query. A single string is
        automatically converted to a one-element set.
    whatif : str or set of str
        Variable(s) whose values are changed in the counterfactual scenario.
        A single string is automatically converted to a one-element set.
    profile : dict of str → str, optional
        The factual observation as ``{variable_name: value_name}``. This
        grounds the counterfactual (step 1: abduction). Default is empty
        (no factual observation).
    values : dict of str → str, optional
        Counterfactual values for the *whatif* variables as
        ``{variable_name: value_name}``. If omitted, the full distribution
        over all *whatif* values is returned.

    Returns
    -------
    pyagrum.Tensor
        The counterfactual distribution P(on | do(whatif)) evaluated on the
        twin model, optionally sliced by *values*.

    Examples
    --------
    >>> import pyagrum as gum
    >>> import pyagrum.causal as csl
    >>> bn = gum.BayesNet.fastPrototype('X->Y->Z')
    >>> cm = csl.CausalModel(bn)
    >>> t = csl.counterfactual(cm, on='Z', whatif='X',
    ...                        profile={'Y': 'True'}, values={'X': 'False'})
    """
    p=_counterfactual(cm, on, whatif,
                           profile if profile is not None else {},
                           values  if values  is not None else {})
    p._model=cm
    return p

def counterfactualModel(cm, profile=None, whatif=None):
    """
    Build the twin causal model for a counterfactual query.

    Implements steps 1-2 of Pearl's three-step counterfactual algorithm:
    compute the posterior of parentless (idiosyncratic) nodes in the
    observational BN given *profile* as evidence, then replace their priors
    in a copy of the model with those posteriors.

    Parameters
    ----------
    cm : pyagrum.causal.CausalModel
        The original causal model.
    profile : dict of str → str, optional
        The factual observation as ``{variable_name: value_name}``.
        Default is empty (no observation; the twin model equals the original).
    whatif : str or set of str, optional
        Intervened variable(s) in the counterfactual scenario. These are
        excluded from the set of idiosyncratic nodes that get updated.
        A single string is automatically converted to a one-element set.
        Default is empty.

    Returns
    -------
    pyagrum.causal.CausalModel
        The twin causal model ready for the prediction step.

    Examples
    --------
    >>> import pyagrum as gum
    >>> import pyagrum.causal as csl
    >>> bn = gum.BayesNet.fastPrototype('X->Y->Z')
    >>> cm = csl.CausalModel(bn)
    >>> twin = csl.counterfactualModel(cm, profile={'Y': 'True'}, whatif='X')
    """
    p=_counterfactualModel(cm,
                           profile if profile is not None else {},
                           whatif  if whatif  is not None else set())
    p._model=cm
    return p

%}
