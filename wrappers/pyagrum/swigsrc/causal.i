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


// CausalImpact::toLatex: %feature("shadow") exposes doOperatorPrefix and
// doOperatorSuffix as keyword-only arguments, with defaults read from the
// pyAgrum configuration at call time.
%feature("shadow") gum::CausalImpact::toLatex %{
    def toLatex(self, *, doOperatorPrefix=None, doOperatorSuffix=None):
        if doOperatorPrefix is None:
            doOperatorPrefix = config["causal", "latex_do_prefix"]
        if doOperatorSuffix is None:
            doOperatorSuffix = config["causal", "latex_do_suffix"]
        return $action(self, doOperatorPrefix, doOperatorSuffix)
%}

// CausalImpact constructor: keyword-only on/doing/knowing with str→set coercion.
// $action expands to _pyagrum.new_CausalImpact; swiginit is the SWIG object
// initialisation convention for this project.
%feature("shadow") gum::CausalImpact::CausalImpact %{
    def __init__(self, cm, *, on, doing, knowing=None):
        if isinstance(on, str):
            on = {on}
        if isinstance(doing, str):
            doing = {doing}
        if isinstance(knowing, str):
            knowing = {knowing}
        _pyagrum.CausalImpact_swiginit(self, $action(cm, on, doing,
                                       knowing if knowing is not None else set(),
                                       False))
%}



// LatentDescriptorVector and LatentDescriptorIds are C++-internal types with no
// Python representation. Ignoring them causes SWIG to suppress all overloads
// that use them (including the LatentDescriptorVector constructors of CausalModel),
// keeping the Python error messages free of internal C++ type names.
%ignore gum::LatentDescriptorIds;
%ignore gum::LatentDescriptorVector;


%pythoncode %{
def causalImpact(cm, *, on, doing, knowing=None, values=None):
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
    >>> bn = gum.BayesNet.fastPrototype('X->Y->Z')
    >>> cm = gum.CausalModel(bn)
    >>> formula, tensor, expl = gum.causalImpact(cm, on='Z', doing='X')
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

def counterfactual(cm, *, on, whatif, profile=None, values=None):
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
    >>> bn = gum.BayesNet.fastPrototype('X->Y->Z')
    >>> cm = gum.CausalModel(bn)
    >>> t = gum.counterfactual(cm, on='Z', whatif='X',
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
    >>> bn = gum.BayesNet.fastPrototype('X->Y->Z')
    >>> cm = gum.CausalModel(bn)
    >>> twin = gum.counterfactualModel(cm, profile={'Y': 'True'}, whatif='X')
    """
    p=_counterfactualModel(cm,
                           profile if profile is not None else {},
                           whatif  if whatif  is not None else set())
    p._model=cm
    return p

%}


%extend gum::CausalImpact{
%pythoncode %{
    def print_ast(self):
      """
      Print the AST of a CausalImpact function in a human readable way.

      Parameters
      ----------
        impact : pyagrum.CausalImpact
          the function whose AST we want to print
      """
      def print_ast_rec(node, name="", indent="  ", last=True):
        marker = "└─ " if last else "├─ "

        if isinstance(node, dict):
            label = node.get("op", name)
            print(f"{indent}{marker}{label}")

            indent += "   " if last else "│  "

            children = [(k, v) for k, v in node.items() if k != "op"]

            for i, (key, value) in enumerate(children):
                is_last = i == len(children) - 1
                print_ast_rec(value, name=key, indent=indent, last=is_last)
        else:
            print(f"{indent}{marker}{name}: {node}")

      def print_title(label:"AST"):
        width = len(label) + 2
        print(f"┌{'─' * width}┐")
        print(f"│ {label} │")
        print(f"└{'─' * width}┘")
        return int(width/2)

      def get_title(f:pyagrum.CausalImpact):
        label=f"P({','.join(f.onNames())}|do({','.join(f.doingNames())})"
        if len(f.knowingNames())>0:
              label+=f",{','.join(f.knowingNames())}"
        label+=")"
        return label

      print_ast_rec(self.toDict(),indent=" "*print_title(get_title(self)))
%}
}
