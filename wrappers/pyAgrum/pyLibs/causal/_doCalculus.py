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
This file computes the causal impact of intervention in a causal model
"""
import itertools as it
from typing import List, Set, Optional, Union

import pyAgrum

from pyAgrum.causal._types import NameSet,NodeSet
from pyAgrum.causal._dSeparation import dSep_reduce, isDSep, ancester
from pyAgrum.causal._doAST import ASTtree, ASTdiv, ASTjointProba, ASTsum, ASTmult, ASTposteriorProba, productOfTrees
from pyAgrum.causal._exceptions import HedgeException

from pyAgrum.causal._CausalModel import inducedCausalSubModel, CausalModel
from pyAgrum.causal._CausalFormula import CausalFormula

# pylint: disable=unused-import
import pyAgrum.causal  # for annotations

def doCalculusWithObservation(cm: CausalModel, on: str, doing: NameSet,
                              knowing: Optional[NameSet] = None) -> CausalFormula:
  """
  Compute the CausalFormula for an impact analysis given the causal model, the observed variables and the
  variable on  which there will be intervention.

  :param on: the variables of interest
  :param cm: the causal model
  :param doing: the interventions
  :param knowing: the observations

  :return: the CausalFormula for computing this causal impact
  """
  if knowing is None or len(knowing) == 0:
    return doCalculus(cm, on, doing)

  lOn = list(on)
  iDoing = {cm.idFromName(i) for i in doing}
  iOn = {cm.idFromName(i) for i in lOn}
  iKnowing = {cm.idFromName(i) for i in knowing}

  removedArcs = set()

  for e in iDoing:
    for p in cm.parents(e):
      cm.eraseCausalArc(p, e)
      removedArcs.add((p, e))
  for e in iKnowing:
    for c in cm.children(e):
      cm.eraseCausalArc(e, c)
      removedArcs.add((e, c))
  rg = dSep_reduce(cm, iDoing | iOn | iKnowing)
  for a, b in removedArcs:
    cm.addCausalArc(a, b)

  for i in iKnowing:
    if isDSep(rg, {i}, iOn, iDoing | (iKnowing - {i})):
      try:
        return doCalculusWithObservation(cm, on, doing | {cm.names()[i]}, knowing - {cm.names()[i]})
      except HedgeException:
        pass

  p = doCalculus(cm, on | knowing, doing)
  q = doCalculus(cm, knowing, doing)

  return CausalFormula(cm, ASTdiv(p.root, q.root), on, doing, knowing)


def _cDecomposition(cm: CausalModel) -> List[Set[int]]:
  undi = pyAgrum.UndiGraph()
  s = set(cm.nodes()) - cm.latentVariablesIds()
  for n in s:
    undi.addNodeWithId(n)

  for latent in cm.latentVariablesIds():
    for a, b in it.combinations(cm.children(latent), 2):
      undi.addEdge(a, b)

  def undiCComponent(g, n, se):
    for i in g.neighbours(n):
      if i not in se:
        se.add(i)
        undiCComponent(g, i, se)

  components = []
  while len(s) != 0:
    c = s.pop()
    sc = set([c])
    undiCComponent(undi, c, sc)
    s -= sc
    components.append(sc)
  return components


def _topological_sort(cm: CausalModel) -> List[int]:
  lt = cm.latentVariablesIds()

  dc = {i: len(set(cm.parents(i)) - lt) for i in (set(cm.nodes()) - lt)}

  order = []
  while len(dc) != 0:
    rem = set()
    for i in dc:
      if dc[i] == 0:
        order.append(i)
        rem.add(i)

    for r in rem:

      for c in cm.children(r):
        dc[c] -= 1
      del dc[r]

  return order


def doCalculus(cm: CausalModel, on: Union[str, NameSet], doing: Union[str, NameSet]) -> CausalFormula:
  """
  Compute the CausalFormula for computing an impact analysis given the causal model, the observed variables and the
  variable on
  which there will be intervention. Note that there is no value neither for ``on`` nor for ``doing`` variables

  :param on: the variables of interest
  :param cm: the causal model
  :param doing: the interventions
  :return: the CausalFormula for computing this causal impact
  """
  X = doing if isinstance(doing, set) else {doing}  # set of keys in doing
  Y = on if isinstance(on, set) else {on}

  return CausalFormula(cm, identifyingIntervention(cm, Y, X), on, doing)


def identifyingIntervention(cm: CausalModel, Y: NameSet, X: NameSet, P: ASTtree = None) -> ASTtree:
  """
  Following Shpitser, Ilya and Judea Pearl. 'Identification of Conditional Interventional Distributions.' UAI2006 and
  'Complete Identification Methods for the Causal Hierarchy' JMLR 2008

  :param cm: the causal model
  :param Y: The variables of interest (named following the paper)
  :param X: The variable of intervention (named following the paper)
  :param P: The ASTtree representing the calculus in construction
  :return: the ASTtree representing the calculus
  """
  iX = {cm.idFromName(i) for i in X}
  iY = {cm.idFromName(i) for i in Y}
  iV = set(cm.nodes()) - cm.latentVariablesIds()
  V = {cm.names()[i] for i in iV}

  # 1 ------------------------------------------------
  if len(X) == 0:
    if P is None:
      lY = list(Y)
      return ASTjointProba(lY)

    vy = V - Y
    if len(vy) != 0:
      lvy = list(vy)
      return ASTsum(lvy, P)

    return P

  # 2 -------------------------------------------
  iAnY = set()
  for i in iY:
    ancester(i, cm, iAnY)
  iAnY |= iY
  AnY = {cm.names()[i] for i in iAnY}

  if len(cm.nodes()) != len(AnY):
    ivAny = list(iV - iAnY)

    if P is not None:
      vAny = [cm.names()[j] for j in ivAny]
      P = ASTsum(vAny, P)
    return identifyingIntervention(inducedCausalSubModel(cm, iAnY), Y, X & AnY, P)

  # 3  -----------------------------------------------
  rmArcs = set()

  for e in iX:
    for p in cm.parents(e):
      cm.eraseCausalArc(p, e)
      rmArcs.add((p, e))

  ianY = set()
  for i in iY:
    ancester(i, cm, ianY)
  ianY |= iY
  iW = (iV - iX) - ianY

  for a, b in rmArcs:
    cm.addCausalArc(a, b)

  if len(iW) != 0:
    W = {cm.names()[i] for i in iW}
    return identifyingIntervention(cm, Y, X | W, P)

  gvx = inducedCausalSubModel(cm, iV - iX)
  icd = _cDecomposition(gvx)
  cd = []
  for s in icd:
    cd.append({cm.names()[i] for i in s})

  # 4----------------
  if len(cd) > 1:
    t = identifyingIntervention(cm, cd[0], V - cd[0], P)
    for si in cd[1:]:
      Pp = P.copy() if P is not None else None
      t = ASTmult(identifyingIntervention(cm, si, V - si, Pp), t)

    vyx = V - (X | Y)
    lvyx = list(vyx)

    if len(lvyx) == 0:
      return t
    return ASTsum(lvyx, t)

  S = cd[0]
  iS = icd[0]
  cdg = _cDecomposition(cm)

  # 5-------------------------
  if len(cdg) == 1 and len(cdg[0]) == len(V):
    raise HedgeException(f"Hedge Error: G={V}, G[S]={S}", V, S)

  # 6--------------------------
  gs = inducedCausalSubModel(cm, iS)
  if set(gs.nodes()) - gs.latentVariablesIds() in cdg:
    vpi = []
    prb = []
    to = _topological_sort(cm)
    for v in S:
      vpi = to[:to.index(cm.idFromName(v))]
      nvpi = {cm.names()[i] for i in vpi}

      if len(nvpi) == 0:
        prb.append(ASTjointProba([v]))
      else:
        if P is None:
          prb.append(ASTposteriorProba(cm.causalBN(), {v}, nvpi))
        else:
          prb.append(ASTdiv(P.copy(), ASTsum(v, P.copy())))

    prod = productOfTrees(prb)

    if len(S - Y) == 0:
      return prod

    lsy = list(S - Y)
    ilsy = [cm.idFromName(i) for i in lsy]
    return ASTsum(ilsy, prod)

  # 7------------------------------------------
  for ispr in cdg:
    if iS <= ispr:
      spr = {cm.names()[i] for i in ispr}
      prb = []
      top = _topological_sort(cm)

      for v in spr:
        vpi = top[:top.index(cm.idFromName(v))]
        nvpi = {cm.names()[i] for i in vpi}

        if len(nvpi) == 0:
          prb.append(ASTjointProba([v]))
        else:
          prb.append(ASTposteriorProba(cm.causalBN(), {v}, nvpi))

      P = productOfTrees(prb)
      return identifyingIntervention(inducedCausalSubModel(cm, ispr), Y, X & spr, P)

  return None


def getBackDoorTree(cm: CausalModel, x: str, y: str, zset: NodeSet) -> ASTtree:
  """
  Create an CausalFormula representing a backdoor zset from x to y in the causal mode lcm

  :param cm: causal model
  :param x: impacting node
  :param y: impacted node
  :param zset: backdoor set
  :return: the ASTtree for the backoor criteria
  """
  zp = [cm.names()[i] for i in zset]
  return ASTsum(zp,
                ASTmult(ASTposteriorProba(cm.causalBN(), {y}, set([x] + zp)),
                        ASTjointProba(zp))
                )


def getFrontDoorTree(cm: CausalModel, x: str, y: str, zset: NodeSet) -> ASTtree:
  """
  Create an AdsT representing a frontdoor zset from x to y in the causal model

  :param cm: causal model
  :param x: impacting node
  :param y: impacted node
  :param zset: frontdoor set
  :return: the ASTtree for the frontdoot critreroia
  """
  zp = [cm.names()[i] for i in zset]
  return ASTsum(zp, ASTmult(ASTposteriorProba(cm.causalBN(), set(zp), {x}),
                            ASTsum([x],
                                   ASTmult(ASTposteriorProba(cm.causalBN(), {y}, set([x] + zp)),
                                           ASTjointProba([x]))
                                   )
                            )
                )
