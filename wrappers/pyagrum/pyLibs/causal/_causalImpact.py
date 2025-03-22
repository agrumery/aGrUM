############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

"""
This file gives an API for causal inference
"""
from typing import Union, Optional, Dict, Tuple, Set

import pyagrum

from pyagrum.causal._types import NameSet
from pyagrum.causal._dSeparation import isDSep
from pyagrum.causal._doAST import ASTposteriorProba

from pyagrum.causal._exceptions import HedgeException

from pyagrum.causal._CausalModel import CausalModel
from pyagrum.causal._CausalFormula import CausalFormula, _getLabelIdx
from pyagrum.causal._doCalculus import doCalculusWithObservation, doCalculus, getFrontDoorTree, getBackDoorTree

# pylint: disable=unused-import
import pyagrum.causal  # for annotations


def causalImpact(cm: CausalModel,
                 on: Union[str, NameSet],
                 doing: Union[str, NameSet],
                 knowing: Optional[NameSet] = None,
                 values: Optional[Dict[str, int]] = None) -> Tuple[
  'pyagrum.causal.CausalFormula', 'pyagrum.Tensor', str]:
  """
  Determines the causal impact of interventions.

  Determines the causal impact of the interventions specified in ``doing`` on the single or list of variables ``on``
  knowing the states of the variables in ``knowing`` (optional). These last parameters is dictionary <variable
  name>:<value>. The causal impact is determined in the causal DAG ``cm``.
  This function returns a triplet with a latex format formula used to compute the causal impact, a tensor
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
  Tuple[CausalFormula,pyagrum.Tensor,str]
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

  formula, tensor, explanation = _causalImpact(cm, son, sdoing, sk)

  # no need to contextualize the tensor
  if tensor is None or values is None:
    potfinal = tensor
  else:
    sv = set(tensor.names)
    extract_values = {k: _getLabelIdx(cm.observationalBN(), k, v)
                      for k, v in values.items() if k in sv}
    potfinal = tensor.extract(extract_values)

  # doCalculous can change doing and knowing
  if formula is not None:
    formula._setDoing(sdoing)
    formula._setKnowing(sk)

  return formula, potfinal, explanation


def _causalImpact(cm: CausalModel, on: Union[str, NameSet],
                  doing: Union[str, NameSet],
                  knowing: Union[str, NameSet]) \
   -> Tuple['pyagrum.causal.CausalFormula', 'pyagrum.Tensor', str]:
  """
  Determines the causal impact of interventions.

  Determines the causal impact of the interventions specified in ``doing`` on the single or list of variables ``on``
  knowing the states of the variables in ``knowing`` (optional). The causal impact is determined in the causal DAG
  ``cm``.
  This function returns a triplet with a latex format formula used to compute the causal impact, a tensor
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
  Tuple[CausalFormula,pyagrum.Tensor,str]
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

  adj = adj.reorganize(lv)  # sumIn(lv).reorganize(lv)
  explain = "Do-calculus computations"
  return ar, adj, explain


def counterfactualModel(cm: CausalModel, profile: Union[Dict[str, int], type(None)],
                        whatif: Union[str, Set[str]]) -> CausalModel:
  """
  Determines the estimation of the twin model following the three steps algorithm from "The Book Of Why" (Pearl 2018) chapter 8 page 253.

  This is done according to the following algorithm:
      -Step 1: calculate the posterior probabilities of idiosyncratic nodes (parentless nodes - whatif-latent variables) in the BN with which we created the causal model with  "profile" as evidence.
      -Step 2 : We replace in the original BN the prior probabilities of idiosyncratic nodes with tensors calculated in step 1 (it will spread to the causal model)

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

  # calculate the posterior probability of each idiosyncratic factor knowing the profile in the original BN
  # posteriors will be a dict {factor : posterior probability knowing the profile}
  posteriors = dict.fromkeys(idiosyncratic)
  ie = pyagrum.LazyPropagation(bn)
  ie.setEvidence(profile)
  ie.makeInference()
  for factor in idiosyncratic:
    posteriors[factor] = ie.posterior(factor)

  # Step 2 : We replace the prior probabilities of idiosyncratic nodes with tensors calculated in step 1 in the BN
  # Saving the original CPTs of idiosyncratic variables
  for factor in idiosyncratic:
    bn.cpt(factor).fillWith(posteriors[factor])

  return twincm


def counterfactual(cm: CausalModel, profile: Union[Dict[str, int], type(None)], on: Union[str, Set[str]],
                   whatif: Union[str, Set[str]],
                   values: Union[Dict[str, int], type(None)] = None) -> "pyagrum.Tensor":
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

  This function returns the tensor calculated in step 3, representing the probability distribution of  "on" given
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
  pyagrum.Tensor
    the computed counterfactual impact
  """
  # Step 1 and 2 : create the twin causal model
  twincm = counterfactualModel(cm, profile, whatif)

  # Step 3 : operate the intervention in the causal model based on bn
  _, adj, _ = causalImpact(
    twincm, on=on, doing=whatif, values=values)
  # cslnb.showCausalImpact(cm,on = on,whatif=whatif,values=values)

  # adj is using variables from twincm. We copy it in a Tensor using variables of cm
  res = pyagrum.Tensor()
  for v in adj.names:
    res.add(cm.observationalBN().variableFromName(v))
  res.fillWith(adj)
  return res
