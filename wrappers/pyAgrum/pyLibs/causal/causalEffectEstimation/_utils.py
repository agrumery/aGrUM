# (c) Copyright 2020-2024 by Pierre-Henri Wuillemin(@LIP6)  (pierre-henri.wuillemin@lip6.fr)
# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and
# without fee or royalty is hereby granted, provided
# that the above copyright notice appear in all copies
# and that both that copyright notice and this permission
# notice appear in supporting documentation or portions
# thereof, including modifications, that you make.

# THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
# OR PERFORMANCE OF THIS SOFTWARE!

import pyAgrum as gum
import pyAgrum.causal as csl

from collections import deque

EXCEPTION_TEXT = "\n(Call `.use[estimator_name]()` to select an estimator.)"

RCT_ESTIMATORS_LIST = "\n- CausalModelEstimator"\
    "\n- DM"

BACKDOOR_ESTIMATORS_LIST = "\n- CausalModelEstimator"\
    "\n- SLearner"\
    "\n- TLearner"\
    "\n- XLearner"\
    "\n- PStratification"\
    "\n- IPW"

FRONTDOOR_ESTIMATORS_LIST = "\n- CausalModelEstimator"\
    "\n- SimplePlugIn"\
    "\n- GeneralizedPlugIn"

IV_ESTIMATORS_LIST = "\n- CausalModelEstimator"\
    "\n- Wald"\
    "\n- WaldIPW"\
    "\n- NormalizedWaldIPW"\
    "\n- TSLS"

class MisspecifiedAdjustmentError(ValueError):
    def __init__(self, before=None) -> None:
        self.message = (
            f"Please select a valid adjustment before {before}. \n"\
            "The supported adjustments are:"\
            "\n- randomized controlled trial\t\t: call `.useRCTAdjustment()`"\
            "\n- backdoor\t\t\t\t: call `.useBackdoorAdjustment()`"\
            "\n- generalized frontdoor\t\t\t: call `.useFrontdoorAdjustment()`"\
            "\n- generalized instrumental variable\t: call `.useIVAdjustment()`"
        )
        super().__init__(self.message)

class MisspecifiedLearnerError(ValueError):
    def __init__(self, learner_name=None) -> None:
        self.message = (
            f"The specified learner string: `{learner_name}` is not "\
            "recognized or does not correspond to any supported learner.\n"
            "Consider passing the appropriate scikit-learn estimator object "
            "directly, which should implement the `.fit()`, `.predict()`, "\
            "and `.predict_proba()` methods, or use one of the following "\
            "supported learner strings:"
            "\n- LinearRegression"
            "\n- Ridge"
            "\n- Lasso"
            "\n- PoissonRegressor"
            "\n- DecisionTreeRegressor"
            "\n- RandomForestRegressor"
            "\n- GradientBoostingRegressor"
            "\n- AdaBoostRegressor"
            "\n- SVR"
            "\n- KNeighborsRegressor"
            "\n- XGBRegressor"
            "\n- XGBClassifier"
        )
        super().__init__(self.message)

class EmptyConditionError(ZeroDivisionError):
    def __init__(self) -> None:
        self.message = "No matching instances found in the data for the "\
            "provided conditions.\nPlease ensure the conditions "\
            "are correctly specified or consider using a Pandas "\
            "DataFrame with these conditions containing intervention "\
            "and control instances for estimation purposes."
        super().__init__(self.message)

class InvalidConditionError(ValueError):
    def __init__(self) -> None:
        self.message = "Invalid Conditional.\n"\
            "Please use a Pandas DataFrame, string "\
            "or Nonetype as the conditional."
        super().__init__(self.message)

class RCTError(ValueError):
    def __init__(self, estimator_name=None) -> None:
        self.message = (
            f"The specified estimator: '{estimator_name}' is not supported "\
            "by the Randomized Controlled Trial criterion. "\
            "\nPlease choose a supported estimator:"
            + RCT_ESTIMATORS_LIST +
            "\nIf the outcome variable is a cause of other covariates in the "
            "causal graph, Backdoor estimators may also be used."
            + EXCEPTION_TEXT
        )
        super().__init__(self.message)

class BackdoorError(ValueError):
    def __init__(self, estimator_name=None) -> None:
        self.message = (
            f"The specified estimator: '{estimator_name}' is not supported "\
            "by the backdoor criterion. "
            "\nPlease choose a supported estimator:"
            + BACKDOOR_ESTIMATORS_LIST
            + EXCEPTION_TEXT
        )
        super().__init__(self.message)

class FrontdoorError(ValueError):
    def __init__(self, estimator_name=None) -> None:
        self.message = (
            f"The specified estimator: '{estimator_name}' is not supported "\
            "by the (genralized) frontdoor criterion. "
            "\nPlease choose a supported estimator:"
            + FRONTDOOR_ESTIMATORS_LIST
            + EXCEPTION_TEXT
        )
        super().__init__(self.message)

class IVError(ValueError):
    def __init__(self, estimator_name=None) -> None:
        self.message = (
            f"The specified estimator: '{estimator_name}' is not supported "\
            "by the (conditional) instrumental variable criterion. "
            "\nPlease choose a supported estimator:"
            + IV_ESTIMATORS_LIST
            + EXCEPTION_TEXT
        )
        super().__init__(self.message)

def RCT(
    causal_model: csl.CausalModel,
    intervention: str,
    outcome: str
) -> set[str] | None:
    """
    Determine the Randomized Controlled Trial (RCT) adjustment.

    Parameters
    ----------
    intervention: str
        Intervention (treatment) variable.
    outcome: str
        Outcome variable.

    Returns
    -------
    set[str] or None
        Set with the names of the confounders if ignorability.
        None if ignorability is not satisfied.
    """
    cbn_without_T_Y = gum.BayesNet(causal_model.causalBN())
    t = cbn_without_T_Y.idFromName(intervention)
    y = cbn_without_T_Y.idFromName(outcome)

    if cbn_without_T_Y.existsArc(t, y):
        cbn_without_T_Y.eraseArc(t, y)

    if csl._dSeparation.isDSep(cbn_without_T_Y, {t}, {y}, set()):
        return {cbn_without_T_Y.variable(pa).name() \
            for pa in cbn_without_T_Y.parents(y)}
    else:
        return None

def _verifyFrontDoorDSep(
    cbn: gum.BayesNet,
    t: int,
    y: int,
    M: set[int],
    W: set[int]
) -> bool:
    """
    Verify the generalized frontdoor adjustment d-Sepatation assumptions.

    Parameters
    ----------
    cbn: gum.BayesNet
        The causal Baysian Network.
    t: int
        The intervention node ID.
    y: int
        The outcome node ID.
    M: set[int]
        The set of mediator node IDs.
    W: set[int]
        The set of confounder node IDs.

    Returns
    -------
    bool
        True if the M is d-Sep. from {t} in the mutilated graph without
        the arcs t->M, and M is d-Sep. from {y} in the mutilated graph
        without the arcs M->{y} and t and y are not neighbors.
    """

    cbn_without_T_M = gum.BayesNet(cbn)
    cbn_without_M_Y = gum.BayesNet(cbn)

    for m in M:
        if cbn_without_T_M.existsArc(t, m):
            cbn_without_T_M.eraseArc(t, m)
        if cbn_without_M_Y.existsArc(m, y):
            cbn_without_M_Y.eraseArc(m, y)

    res =  csl._dSeparation.isDSep(
        cbn_without_T_M, {t}, M, W
    ) and csl._dSeparation.isDSep(
        cbn_without_M_Y, {y}, M, W | {t}
    ) and t not in cbn.parents(y) | cbn.children(y)

    return res

def generalizedFrontDoor(
    causal_model: csl.CausalModel,
    intervention: str,
    outcome: str
) -> tuple[set[str]] | None:
    """
    Identify the generalised frontdoor adjustment set and covariates.

    Parameters
    ----------
    intervention: str
        Intervention (treatment) variable.
    outcome: str
        Outcome variable.

    Returns
    -------
    tuple[set[str]] or None
        Set with the names of the mediators,
        set with the names of covariates, or None if not applicable.
    """

    obn = causal_model.observationalBN()
    cbn = causal_model.causalBN()

    mediators = csl._doorCriteria.nodes_on_dipath(
        obn,
        obn.idFromName(intervention),
        obn.idFromName(outcome)
    )
    mediators = {obn.variable(m).name() for m in mediators}

    confounders = set()

    for m in mediators:
        backdoor_T_M =  causal_model.backDoor(intervention, m)
        backdoor_M_Y =  causal_model.backDoor(m, outcome)
        backdoor_T_M = set() if backdoor_T_M is None else backdoor_T_M
        backdoor_M_Y = set() if backdoor_M_Y is None else backdoor_M_Y
        confounders |=  backdoor_T_M | backdoor_M_Y

    confounders = confounders - {intervention}

    # Clone with latent variables:
    # Sometime the causal structure is changed while cloning,
    # so extra operations must be made
    mutilated_causal_model = causal_model.clone()

    for id in causal_model.latentVariablesIds():
        childrens = cbn.children(id)
        childrens = {cbn.variable(c).name() for c in childrens}
        if cbn.variable(id).name() not in \
            mutilated_causal_model.names().values():
            mutilated_causal_model.addLatentVariable(
                cbn.variable(id).name(), tuple(childrens)
            )

    for c in confounders:
        if mutilated_causal_model.existsArc(c, intervention):
            mutilated_causal_model.eraseCausalArc(c, intervention)
        if mutilated_causal_model.existsArc(c, outcome):
            mutilated_causal_model.eraseCausalArc(c, outcome)
        for m in mediators:
            if mutilated_causal_model.existsArc(c, m):
                mutilated_causal_model.eraseCausalArc(c, m)

    frontdoor = mutilated_causal_model.frontDoor(
        cause=intervention,
        effect=outcome
    )


    valid_fd = _verifyFrontDoorDSep(
            cbn,
            cbn.idFromName(intervention),
            cbn.idFromName(outcome),
            {cbn.idFromName(m) for m in mediators},
            {cbn.idFromName(m) for m in confounders},
        )

    return (None, None) if frontdoor is None or len(mediators) == 0 \
        or not valid_fd else (frontdoor, confounders)

def _findPath(
    G: gum.UndiGraph,
    a: int,
    b: int,
) -> list[int]:
    """
    Find a path in the mixed graph `G` from node `a` to node `b`.

    Parameters
    ----------
    G: gum.MixedGraph
        The graph.
    a: int
        The starting node ID.
    b: int
        The ending node ID.

    Returns
    -------
    list[int]
        The path from node `a` to `b`.
    """

    stack = deque()
    stack.append((a, [a]))
    visited = set()

    while stack:
        (node, path) = stack.pop()

        if node == b:
            return path

        if node not in visited:
            visited.add(node)

            for neighbor in G.neighbours(node):
                if neighbor not in visited:
                    stack.append((neighbor, path + [neighbor]))

    return []

def _nearestSeparator(
    obn: gum.BayesNet,
    cbn: gum.BayesNet,
    t: int,
    y: int,
    z: int
) -> set[int]:
    """
    Find the nearest separator set in the `causal_model` according to `(y,w)`.

    (see https://www.ijcai.org/Proceedings/15/Papers/457.pdf)

    Parameters
    ----------
    causal_model: csl.CausalModel
        The causal graph.
    t: int
        The intervention node ID.
    y: int
        The outcome node ID.
    z: int
        The instrument node ID.

    Returns
    -------
    set[int]
        The nearest separator set in the mutilated graph of
        `causal_model` with respect to `t`.
    """

    M = obn.nodes()
    W = set()

    moralized_ancestral_graph = cbn.moralizedAncestralGraph({z, y})

    while True:

        # Moralized Graph controlling for W
        csl._dSeparation._removeZ(moralized_ancestral_graph, W)

        path = _findPath(moralized_ancestral_graph, y, z)

        if path == list() or set(path[1:-1]) & M == set():
            break

        w = next((node for node in path[1:-1] if node in M), None)
        if w is not None:
            W.add(w)


    if csl._dSeparation.isDSep(cbn, {z}, {y}, W):
        return W
    else:
        return None

def _ancestralInstrument(
    causal_model: csl.CausalModel,
    t: int,
    y: int,
    z: int
) -> set[int]:
    """
    Find the ancetral instrument conditioning set `W` in the `causal_model`
    with `t` as intervention, `y` as outcome and `z` as instrument.

    (see https://www.ijcai.org/Proceedings/15/Papers/457.pdf)

    Parameters
    ----------
    causal_model: csl.CausalModel
        The causal graph.
    t: int
        The intervention node ID.
    y: int
        The outcome node ID.
    z: int
        The instrument node ID.

    Returns
    -------
    set[int]
        the ancetral instrument conditioning set `W`.
    """

    mutilated_obn = gum.BayesNet(causal_model.observationalBN())
    mutilated_cbn = gum.BayesNet(causal_model.causalBN())

    if mutilated_obn.existsArc(t, y):
        mutilated_obn.eraseArc(t, y)
    if mutilated_cbn.existsArc(t, y):
        mutilated_cbn.eraseArc(t, y)

    W = _nearestSeparator(mutilated_obn, mutilated_cbn, t, y, z)
    if W is None \
        or bool(W & mutilated_cbn.descendants(y)) \
        or t in W:
        return None
    elif not csl._dSeparation.isDSep(mutilated_cbn, {z}, {t}, W):
        return W - {t}
    else:
        return None

def instrumentalVariable(
    causal_model: csl.CausalModel,
    intervention: str,
    outcome: str
) -> tuple[set[str], set[str]]:
    """
    Identifies the instrumental variables and covariates, using ancestral
    instruments.

    (see https://www.ijcai.org/Proceedings/15/Papers/457.pdf)
    (see https://ftp.cs.ucla.edu/pub/stat_ser/r303-reprint.pdf)

    Parameters
    ----------
    intervention: str
        Intervention (treatment) variable.
    outcome: str
        Outcome variable.

    Returns
    ------
    tuple[set[str], set[str]] or None
        Set with the names of the instrumental variables,
    """

    obn = causal_model.observationalBN()

    t = intervention
    y = outcome
    if not isinstance(intervention, int):
        t = obn.idFromName(intervention)

    if not isinstance(outcome, int):
        y = obn.idFromName(outcome)

    potential_instruments = obn.parents(intervention)

    for z in potential_instruments:
        W = _ancestralInstrument(causal_model, t, y, z)
        if W is not None:
            return (
                obn.variable(z).name(),
                {obn.variable(w).name() for w in W}
            )
    return (None, None)
