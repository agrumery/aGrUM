# -*- coding: utf-8 -*-
# (c) Copyright by Pierre-Henri Wuillemin, UPMC, 2011  (pierre-henri.wuillemin@lip6.fr)

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

"""
This file gives an API for causal inference
"""
from ._doorCriteria import *
from ._dSeparation import *
from ._doAST import *

from ._exceptions import *
from ._CausalModel import CausalModel
from ._CausalFormula import CausalFormula, _getLabelIdx
from ._doCalculus import doCalculusWithObservation, doCalculus, getFrontDoorTree, getBackDoorTree


def causalImpact(cm: CausalModel, on: Union[str, NameSet],
                 doing: Union[str, NameSet],
                 knowing: Optional[NameSet] = None,
                 values: Optional[Dict[str, int]] = None) -> Tuple[CausalFormula, gum.Potential, str]:
  """
  Determines the causal impact of interventions.

  Determines the causal impact of the interventions specified in ``doing`` on the single or list of variables ``on``
  knowing the states of the variables in ``knowing`` (optional). These last parameters is dictionary <variable
  name>:<value>. The causal impact is determined in the causal DAG ``cm``.
  This function returns a triplet with a latex format formula used to compute the causal impact, a potential
  representing the probability distribution of ``on``  given the interventions and observations as parameters,
  and an explanation of the method allowing the identification. If there is no impact, the joint probability of
  ``on`` is simply returned. If the impact is not identifiable the formula and the adjustment will be ``None`` but an
  explanation  is still given.

  :param cm: causal model
  :param on: variable name or variable names set
  :param doing: variable name or variable names set
  :param knowing: variable names set
  :param values: Dictionary
  :return: the CausalFormula, the computation, the explanation
  """
  # Checking the args
  son = {on} if isinstance(on, str) else on
  sdoing = {doing} if isinstance(doing, str) else doing
  sk = set() if knowing is None else knowing

  # verifies that when len()==0, args are empty sets
  if len(son) == 0:
    son = set()
  if len(sdoing) == 0:
    sdoing = set()
  if len(sk) == 0:
    sk = set()

  total = {cm.observationalBN().variable(cm.observationalBN().idFromName(i)).name() for i in son | sdoing | sk}

  if values is not None:
    for k in values.keys():
      if k not in total:
        raise (ValueError, f"{k} is not in the query arguments.")

  if len(son & sdoing & sk) > 0:
    raise (ValueError, f"The 3 parts of the query (on, doing, knowing) must not intersect.")

  formula, potential, explanation = _causalImpact(cm, son, sdoing, sk)

  # no need to contextualize the potential
  if potential is None or values is None:
    return formula, potential, explanation

  sv = set(potential.var_names)
  extract_values = {k: _getLabelIdx(cm.observationalBN(), k, v) for k, v in values.items() if k in sv}
  potextract = potential.extract(extract_values)
  return formula, potextract, explanation


def _causalImpact(cm: CausalModel, on: NameSet,
                  doing: NameSet,
                  knowing: NameSet) -> Tuple[CausalFormula, gum.Potential, str]:
  """
  Determines the causal impact of interventions.

  Determines the causal impact of the interventions specified in ``doing`` on the single or list of variables ``on``
  knowing the states of the variables in ``knowing`` (optional). The causal impact is determined in the causal DAG
  ``cm``.
  This function returns a triplet with a latex format formula used to compute the causal impact, a potential
  representing the probability distribution of ``on``  given the interventions and observations as parameters,
  and an explanation of the method allowing the identification. If there is no impact, the joint probability of
  ``on`` is simply returned. If the impact is not identifiable the formula and the adjustment will be ``None`` but an
  explanation  is still given.

  :param cm: causal model
  :param on: variable name or variable names set
  :param doing: variable name or variable names set
  :param knowing: variable names set
  :return: the latex representation, the computation, the explanation
  """
  nY = [y for y in on]
  iY = [cm.observationalBN().idFromName(i) for i in nY]

  nDo = [d for d in doing]
  iDo = [cm.observationalBN().idFromName(i) for i in nDo]

  if knowing is None:
    nK = list()
  else:
    nK = [k for k in knowing]
  sK = {cm.observationalBN().idFromName(i) for i in nK}

  # Null causal effect (different from non identifiable causal effect)
  if isDSep(cm, set(iDo), set(iY), sK | cm.latentVariablesIds()):
    explain = "No causal effect of X on Y, because they are d-separated "
    explain += "(conditioning on the observed variables if any)."
    ar = CausalFormula(cm, ASTposteriorProba(cm.causalBN(), set(nY), set(nK)), on, doing, knowing)
    adj = ar.eval()
    return ar, adj.reorganize([v for v in nY + nDo + nK if v in adj.var_names]), explain

  # Front or Back door
  if len(iDo) == 1 and len(nY) == 1 and len(nK) == 0:
    for bd in backdoor_generator(cm, iDo[0], iY[0], cm.latentVariablesIds()):
      ar = CausalFormula(cm, getBackDoorTree(cm, nDo[0], nY[0], bd), on, doing, knowing)
      adj = ar.eval()
      explain = "backdoor " + str([cm.causalBN().variable(i).name() for i in bd]) + " found."
      return ar, adj.reorganize([v for v in nY + nDo + nK if v in adj.var_names]), explain

    for fd in frontdoor_generator(cm, iDo[0], iY[0], cm.latentVariablesIds()):
      ar = CausalFormula(cm, getFrontDoorTree(cm, nDo[0], nY[0], fd), on, doing, knowing)
      adj = ar.eval()
      explain = "frontdoor " + str([cm.causalBN().variable(i).name() for i in fd]) + " found."
      return ar, adj.reorganize([v for v in nY + nDo + nK if v in adj.var_names]), explain

  # Go for do-calculus
  try:
    if len(nK) == 0:
      ar = doCalculus(cm, on, set(nDo))
    else:
      ar = doCalculusWithObservation(cm, on, set(nDo), set(nK))
  except HedgeException as h:
    return None, None, h.message

  adj = ar.eval()
  explain = "Do-calculus computations"

  return ar, adj.reorganize([v for v in nY + nDo + nK if v in adj.var_names]), explain
