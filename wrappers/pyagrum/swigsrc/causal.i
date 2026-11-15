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

// causalImpact and counterfactual are renamed with a leading underscore so that the
// %pythoncode wrappers below can expose them with explicit keyword arguments and type
// coercion (str→set for on/doing/knowing).
// %feature("kwargs") is NOT used here: it causes SIGSEGV on inline-constructed
// temporaries because the **kwargs dispatch can drop the refcount of arguments
// (e.g. gum.CausalModel(edex)) before the C++ call completes.
%rename(_causalImpact) gum::causalImpact;
%rename(_counterfactual) gum::counterfactual;
// counterfactualModel and Counterfactual::counterFactualModel have overloads deprecated after 3.3.2
// (with a profile), and Counterfactual::impact is deprecated after 3.3.2: they are not wrapped
// (calling them from the generated C++ would emit deprecation warnings). The Python functions
// and methods are defined in %pythoncode (here and in causal_after_templates.i) on top of
// Counterfactual._twinNetwork, and emit the Python DeprecationWarning themselves.
%ignore gum::counterfactualModel;
%ignore gum::Counterfactual< double >::counterFactualModel;
%ignore gum::Counterfactual< double >::impact;

// DoorCriteria's two static methods are renamed the same way, so the
// %extend %pythoncode kwargs wrappers in causal_after_templates.i can call
// them back as plain class-qualified names (DoorCriteria._enumerate...Sets)
// instead of reaching into this module's compiled extension by hand.
%rename(_enumerateBackdoorSets) gum::DoorCriteria::enumerateBackdoorSets;
%rename(_enumerateFrontdoorSets) gum::DoorCriteria::enumerateFrontdoorSets;

%ignore *::id2name;
%ignore *::root;
%ignore gum::CausalImpact<double>::getResult;

// CausalImpact::toLatex: %feature("shadow") exposes doOperatorPrefix and
// doOperatorSuffix as keyword-only arguments, with defaults read from the
// pyAgrum configuration at call time.
%feature("shadow") gum::CausalImpact::toLatex %{
    def toLatex(self, *, doOperatorPrefix=None, doOperatorSuffix=None):
        """
        Return a LaTeX string representation of the causal impact formula.

        Parameters
        ----------
        doOperatorPrefix : str, optional
            LaTeX prefix for the do-operator. Defaults to the value in pyAgrum config.
        doOperatorSuffix : str, optional
            LaTeX suffix for the do-operator. Defaults to the value in pyAgrum config.

        Returns
        -------
        str
            LaTeX representation of the causal impact expression.
        """
        if doOperatorPrefix is None:
            doOperatorPrefix = pyagrum.config["causal", "latexDoPrefix"]
        if doOperatorSuffix is None:
            doOperatorSuffix = pyagrum.config["causal", "latexDoSuffix"]
        return $action(self, doOperatorPrefix, doOperatorSuffix)
%}

// CausalImpact constructor: keyword-only on/doing/knowing with str→set coercion.
// $action expands to _cmcpp.new_CausalImpact; swiginit is the SWIG object
// initialisation convention for this project. _cmcpp (not _pyagrumcpp) since
// this module (pyagrum.causal_model) is a separate compiled extension from core pyagrum.
%feature("shadow") gum::CausalImpact::CausalImpact %{
    def __init__(self, cm, *, on, doing, knowing=None):
        if isinstance(on, str):
            on = {on}
        if isinstance(doing, str):
            doing = {doing}
        if isinstance(knowing, str):
            knowing = {knowing}
        _cmcpp.CausalImpact_swiginit(self, $action(cm, on, doing,
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
    cm : pyagrum.CausalModel
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
    tuple (pyagrum.CausalImpact, pyagrum.Tensor or None, str)
        - The CausalImpact object encoding the identified formula.
        - The evaluated tensor P(on | do(doing), knowing), or None if the
          effect is not identifiable.
        - A string explaining the identification method used or why
          identification failed.
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
    Compute a counterfactual distribution in the twin network of Balke and Pearl.

    Answers the question: 'Given that we observed *profile*, what would
    *on* have been if *whatif* had been set as specified in *values*?'

    The three steps of Pearl (abduction, action, prediction) are done by a
    single exact inference in the twin network (`pyagrum.counterfactualModel`):
    the profile is observed in the real world, the values are given to the
    copies of *whatif*, and the result is the posterior of the copies of *on*.

    Parameters
    ----------
    cm : pyagrum.CausalModel
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
        The counterfactual distribution of *on* given *profile*, for each value
        of *whatif* (or sliced by *values*).

    Raises
    ------
    pyagrum.InvalidArgument
        If the causal model has latent variables: their distributions,
        needed for the abduction, are unknown.
    """
    p=_counterfactual(cm, on, whatif,
                           profile if profile is not None else {},
                           values  if values  is not None else {})
    p._model=cm
    return p

def counterfactualModel(cm, whatif=None, profile=None):
    """
    Build the twin network of Balke and Pearl for a counterfactual query.

    The twin network :cite:p:`balke1994probabilistic` contains the real
    world (the observational Bayesian network of *cm*) and the
    counterfactual world: a copy ``V*`` of each variable of *whatif* and of
    each of their descendants. A copy of a *whatif* variable has no parent
    and a uniform prior; any other copy has the conditional probability
    table of its original, its parents being the copies of the original
    parents when they exist, the original parents otherwise. The other
    variables, in particular the parentless ones, are shared by both worlds.
    A counterfactual distribution is a posterior in this network: the
    profile is observed in the real world and the counterfactual values are
    given to the copies (see `pyagrum.counterfactual`).

    Parameters
    ----------
    cm : pyagrum.CausalModel
        The original causal model.
    whatif : str or set of str
        Intervened variable(s) in the counterfactual scenario. A single
        string is automatically converted to a one-element set.
    profile : dict of str → str, optional
        Deprecated, ignored: the twin network does not depend on the
        profile.

    Returns
    -------
    pyagrum.CausalModel
        The twin network.

    Raises
    ------
    pyagrum.InvalidArgument
        If the causal model has latent variables (their distributions are
        unknown), or if the name of a copy is already used.

    Warnings
    --------
    The argument ``profile`` is deprecated after 3.3.2: the twin network does
    not depend on it.
    """
    if isinstance(whatif, dict):  # former positional order (cm, profile, whatif)
        whatif, profile = profile, whatif
    if profile is not None:
        import warnings

        warnings.warn("the argument profile of counterfactualModel is deprecated after 3.3.2 (the twin network does "
                      "not depend on it), use counterfactualModel(cm, whatif)", DeprecationWarning, stacklevel=2)
    if whatif is None:
        whatif = set()
    elif isinstance(whatif, str):
        whatif = {whatif}
    p = Counterfactual._twinNetwork(cm, whatif)
    p._model = cm
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
