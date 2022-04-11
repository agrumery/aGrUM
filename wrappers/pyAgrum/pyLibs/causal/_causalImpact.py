# -*- coding: utf-8 -*-
# (c) Copyright by Pierre-Henri Wuillemin (LIP6), 2020  (pierre-henri.wuillemin@lip6.fr)

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
from typing import Union, Optional, Dict, Tuple, Set

import pyAgrum

from pyAgrum.causal._types import NameSet
from pyAgrum.causal._dSeparation import isDSep
from pyAgrum.causal._doAST import ASTposteriorProba

from pyAgrum.causal._exceptions import HedgeException

from pyAgrum.causal._CausalModel import CausalModel
from pyAgrum.causal._CausalFormula import CausalFormula, _getLabelIdx
from pyAgrum.causal._doCalculus import doCalculusWithObservation, doCalculus, getFrontDoorTree, getBackDoorTree

# pylint: disable=unused-import
import pyAgrum.causal  # for annotations


def causalImpact(cm: CausalModel,
                 on: Union[str, NameSet],
                 doing: Union[str, NameSet],
                 knowing: Optional[NameSet] = None,
                 values: Optional[Dict[str, int]] = None) -> Tuple[
  'pyAgrum.causal.CausalFormula', 'pyAgrum.Potential' , str]:
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

  Parameters
  ----------
  cm: CausalModel
    the causal model
  on: str|NameSet
    variable name or variable names set of interest
  doing: str|NameSet
    the interventions
  knowing: str|NameSet
    the observations
  values: Dict[str,int] default=None
    the values of interventions and observations

  Returns
  -------
  Tuple[CausalFormula,pyAgrum.Potential,str]
    the CausalFormula, the computation, the explanation
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

  total = {cm.observationalBN().variable(cm.observationalBN().idFromName(i)).name()
           for i in son | sdoing | sk}

  if values is not None:
    for k in values.keys():
      if k not in total:
        raise ValueError(f"{k} is not in the query arguments.")

  if len(son & sdoing & sk) > 0:
    raise ValueError("The 3 parts of the query (on, doing, knowing) must not intersect.")

  formula, potential, explanation = _causalImpact(cm, son, sdoing, sk)

  # no need to contextualize the potential
  if potential is None or values is None:
    return formula, potential, explanation

  sv = set(potential.names)
  extract_values = {k: _getLabelIdx(cm.observationalBN(), k, v)
                    for k, v in values.items() if k in sv}
  potextract = potential.extract(extract_values)
  return formula, potextract, explanation


def _causalImpact(cm: CausalModel, on: Union[str,NameSet],
                  doing: Union[str,NameSet],
                  knowing: Union[str,NameSet]) \
   -> Tuple['pyAgrum.causal.CausalFormula', 'pyAgrum.Potential', str]:
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

  Parameters
  ----------
  cm: CausalModel
    the causal model
  on: str|Set[str]
    targeted variable(s)
  doing: str|Set[str]
    interventions
  knowing: str|Set[str]
    observations

  Returns
  -------
  Tuple[CausalFormula,pyAgrum.Potential,str]
    the latex representation, the computation, the explanation
  """
  nY = list(on)
  iY = [cm.observationalBN().idFromName(i) for i in nY]

  nDo = list(doing)
  iDo = [cm.observationalBN().idFromName(i) for i in nDo]

  if knowing is None:
    nK = []
  else:
    nK = list(knowing)
  sK = {cm.observationalBN().idFromName(i) for i in nK}

  # Null causal effect (different from non identifiable causal effect)
  if isDSep(cm, set(iDo), set(iY), sK | cm.latentVariablesIds()):
    explain = "No causal effect of X on Y, because they are d-separated "
    explain += "(conditioning on the observed variables if any)."
    ar = CausalFormula(cm, ASTposteriorProba(
      cm.causalBN(), set(nY), set(nK)), on, doing, knowing)
    adj = ar.eval()
    return ar, adj.reorganize([v for v in nY + nDo + nK if v in adj.names]), explain

  # Front or Back door
  if len(iDo) == 1 and len(nY) == 1 and len(nK) == 0:

    # for bd in backdoor_generator(cm, iDo[0], iY[0], cm.latentVariablesIds()):
    bd = cm.backDoor(iDo[0], iY[0], withNames=False)
    if bd is not None:
      ar = CausalFormula(cm, getBackDoorTree(
        cm, nDo[0], nY[0], bd), on, doing, knowing)
      adj = ar.eval()
      explain = "backdoor " + \
                str([cm.causalBN().variable(i).name() for i in bd]) + " found."
      return ar, adj.reorganize([v for v in nY + nDo + nK if v in adj.names]), explain

    # for fd in frontdoor_generator(cm, iDo[0], iY[0], cm.latentVariablesIds()):
    fd = cm.frontDoor(iDo[0], iY[0], withNames=False)
    if fd is not None:
      ar = CausalFormula(cm, getFrontDoorTree(
        cm, nDo[0], nY[0], fd), on, doing, knowing)
      adj = ar.eval()
      explain = "frontdoor " + \
                str([cm.causalBN().variable(i).name() for i in fd]) + " found."
      return ar, adj.reorganize([v for v in nY + nDo + nK if v in adj.names]), explain

  # Go for do-calculus
  try:
    if len(nK) == 0:
      ar = doCalculus(cm, on, set(nDo))
    else:
      ar = doCalculusWithObservation(cm, on, set(nDo), set(nK))
  except HedgeException as h:
    return None, None, h.message

  adj = ar.eval()
  lsum = nY + nDo + nK
  lv = [v for v in lsum if v in adj.names]

  # todo : check why it is possible that some variables are in names and
  # not in lsum ...  (see for instance p213, book of why and
  # https://twitter.com/analisereal/status/1022277416205475841 : should
  # really z be in the last formula ?)
  ssum = set(lsum)
  lv += [v for v in adj.names if v not in ssum]

  adj = adj.reorganize(lv)  # margSumIn(lv).reorganize(lv)
  explain = "Do-calculus computations"
  return ar, adj, explain


def counterfactualModel(cm: CausalModel, profile: Union[Dict[str, int], type(None)],
                        whatif: Union[str, Set[str]]) -> CausalModel:
  """
  Determines the estimation of the twin model following the three steps algorithm from "The Book Of Why" (Pearl 2018) chapter 8 page 253.

  This is done according to the following algorithm:
      -Step 1: calculate the posterior probabilities of idiosyncratic nodes (parentless nodes - whatif-latent variables) in the BN with which we created the causal model with  "profile" as evidence.
      -Step 2 : We replace in the original BN the prior probabilities of idiosyncratic nodes with potentials calculated in step 1 (it will spread to the causal model)

  This function returns the twin CausalModel

  Parameters
  ----------
  cm: CausalModel
  profile: Dict[str,int] default=None
    evidence
  whatif: str|Set[str]
    idiosyncratic nodes

  Returns
  -------
  CausalModel
    the twin CausalModel
  """
  # Step 1 : calculate the posterior probabilities of idiosyncratic nodes knowing the profil

  # whatif can be a string or a set of strings
  if isinstance(whatif, str):
    idWhatif = {whatif}
  else:
    idWhatif = whatif
  idWhatif = set(map(cm.idFromName, idWhatif))

  # get nodes without parents in the causal model
  parentless = set()
  # nodes of the causal model
  nodes = cm.names().keys()
  for node in nodes:
    # if nb parents is equal to zero => parentless node
    if len(cm.parents(node)) == 0:
      parentless.add(node)

  # idiosyncratic factors (specific to and representative of the profile) are parentless - (whatif+latent variables)
  idiosyncratic = parentless.difference(idWhatif)
  idiosyncratic = idiosyncratic.difference(cm.latentVariablesIds())

  # copying the causal model
  twincm = cm.clone()
  bn = twincm.observationalBN()

  # calculate the posterior probability of each idiosyncratic factor knowing the profil in the original BN
  # posteriors will be a dict {factor : posterior probability knowing the profil}
  posteriors = dict.fromkeys(idiosyncratic)
  ie = pyAgrum.LazyPropagation(bn)
  ie.setEvidence(profile)
  ie.makeInference()
  for factor in idiosyncratic:
    posteriors[factor] = ie.posterior(factor)

  # Step 2 : We replace the prior probabilities of idiosyncratic nodes with potentials calculated in step 1 in the BN
  # Saving the original CPTs of idiosyncratic variables
  for factor in idiosyncratic:
    bn.cpt(factor).fillWith(posteriors[factor])

  return twincm


def counterfactual(cm: CausalModel, profile: Union[Dict[str, int], type(None)], on: Union[str, Set[str]],
                   whatif: Union[str, Set[str]],
                   values: Union[Dict[str, int], type(None)] = None) -> "pyAgrum.Potential":
  """
  Determines the estimation of a counterfactual query following the the three steps algorithm from "The Book Of Why"
  (Pearl 2018) chapter 8 page 253.

  Determines the estimation of the counterfactual query: Given the "profile" (dictionary <variable name>:<value>),what
  would variables in "on" (single or list of variables) be if variables in "whatif" (single or list of variables) had
  been as specified in "values" (dictionary <variable name>:<value>)(optional).

  This is done according to the following algorithm:
      -Step 1-2: compute the twin causal model
      -Step 3 : determine the causal impact of the interventions specified in  "whatif" on the single or list of
      variables "on" in the causal model.

  This function returns the potential calculated in step 3, representing the probability distribution of  "on" given
  the interventions  "whatif", if it had been as specified in "values" (if "values" is omitted, every possible value of
  "whatif")

  Parameters
  ----------
  cm: CausalModel
  profile: Dict[str,int] default=None
    evidence
  on: variable name or variable names set
   the variable(s) of interest
  whatif: str|Set[str]
    idiosyncratic nodes
  values: Dict[str,int]
    values for certain variables in whatif.

  Returns
  -------
  pyAgrum.Potential
    the computed counterfactual impact
  """
  # Step 1 and 2 : create the twin causal model
  twincm = counterfactualModel(cm, profile, whatif)

  # Step 3 : operate the intervention in the causal model based on bn
  _, adj, _ = causalImpact(
    twincm, on=on, doing=whatif, values=values)
  # cslnb.showCausalImpact(cm,on = on,whatif=whatif,values=values)

  # adj is using variables from twincm. We copy it in a Potential using variables of cm
  res = pyAgrum.Potential()
  for v in adj.names:
    res.add(cm.observationalBN().variableFromName(v))
  res.fillWith(adj)
  return res
