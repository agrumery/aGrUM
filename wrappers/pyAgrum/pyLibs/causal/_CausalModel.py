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
This file defines a representation for causal model
"""
from __future__ import annotations

import itertools as it
import pyAgrum as gum

from ._types import *


class CausalModel:
  """
  From an observational BNs and the description of latent variables, this class represent a complet causal model
  obtained by adding the latent variables specified in ``latentVarsDescriptor`` to the Bayesian network ``bn``.

  :param bn: a observational bayesian network
  :param latentVarsDescriptor:  list of couples (<latent variable name>, <list of affected variables' ids>).
  :param keepArcs: By default, the arcs between variables affected by a common latent variable will be removed but
         this can be avoided by setting ``keepArcs`` to ``True``
  """

  def __init__(self, bn: gum.BayesNet,
               latentVarsDescriptor: LatentDescriptorList = None,
               keepArcs: bool = False):
    self.__observationalBN = bn

    if latentVarsDescriptor is None:
      latentVarsDescriptor = []

    # we have to redefine those attributes since the __observationalBN may be augmented by latent variables
    self.__causalBN = gum.BayesNet()

    # nodes of BN
    for n in bn.nodes():
      self.__causalBN.add(bn.variable(n), n)

    # arcs on BN
    for x, y in bn.arcs():
      self.__causalBN.addArc(x, y)

    # latent variables and arcs from latent variables
    self.__lat: NodeSet = set()

    for n, ls in latentVarsDescriptor:
      self.addLatentVariable(n, ls, keepArcs)

    self.__names = {nId: self.__causalBN.variable(nId).name() for nId in self.__causalBN.nodes()}

  def addLatentVariable(self, name: str, ls: Tuple[str, str], keepArcs: bool):
    id_latent = self.__causalBN.add(name, 2)  # simplest variable to add : only 2 modalities for latent variables
    self.__lat.add(id_latent)

    for item in ls:
      j = self.__observationalBN.idFromName(item) if isinstance(item, str) else item
      self.addCausalArc(id_latent, j)

    if not keepArcs:
      ils = {self.__observationalBN.idFromName(x) for x in ls}
      for ix, iy in it.combinations(ils, 2):
        if ix in self.__causalBN.parents(iy):
          self.eraseCausalArc(ix, iy)
        elif iy in self.__causalBN.parents(ix):
          self.eraseCausalArc(iy, ix)

  def causalBN(self) -> gum.BayesNet:
    """
    :return: the causal Bayesian network

    :warning: do not infer any computations in this model. It is strictly a structural model
    """
    return self.__causalBN

  def observationalBN(self) -> gum.BayesNet:
    """
    :return: the observational Bayesian network
    """
    return self.__observationalBN

  def parents(self, x: Union[NodeId, str]) -> NodeSet:
    """
    From a NodeId, returns its parent (as a set of NodeId)

    :param x: the node
    :return:
    """
    return self.__causalBN.parents(self.__causalBN.idFromName(x) if isinstance(x, str) else x)

  def children(self, x: Union[NodeId, str]) -> NodeSet:
    """
    :param x: the node
    :return: 
    """
    return self.__causalBN.children(self.__causalBN.idFromName(x) if isinstance(x, str) else x)

  def names(self) -> Dict[NodeId, str]:
    """
    :return: the map NodeId,Name
    """
    return self.__names

  def idFromName(self, name: str) -> NodeId:
    """
    :param name: the name of the variable
    :return: the id of the variable
    """
    return self.__causalBN.idFromName(name)

  def latentVariablesIds(self) -> NodeSet:
    """
    :return: the set of ids of latent variables in the causal model
    """
    return self.__lat

  def eraseCausalArc(self, x, y):
    ix = self.__observationalBN.idFromName(x) if isinstance(x, str) else x
    iy = self.__observationalBN.idFromName(y) if isinstance(y, str) else y
    self.__causalBN.eraseArc(gum.Arc(ix, iy))

  def addCausalArc(self, x, y):
    ix = self.__observationalBN.idFromName(x) if isinstance(x, str) else x
    iy = self.__observationalBN.idFromName(y) if isinstance(y, str) else y
    self.__causalBN.addArc(ix, iy)

  def existsArc(self, x, y) -> bool:
    ix = self.__observationalBN.idFromName(x) if isinstance(x, str) else x
    iy = self.__observationalBN.idFromName(y) if isinstance(y, str) else y
    return self.__causalBN.dag().existsArc(ix, iy)

  def nodes(self):
    return self.__causalBN.nodes()

  def arcs(self) -> ArcSet:
    return self.__causalBN.arcs()


def inducedCausalSubModel(cm: CausalModel, sns: NodeSet = None) -> CausalModel:
  if sns is None:
    sns = cm.nodes()
  nodes = sns - cm.latentVariablesIds()

  bn = gum.BayesNet()

  for n in nodes:
    bn.add(cm.observationalBN().variable(n), n)

  for x, y in cm.arcs():
    if y in nodes:
      if x in nodes:
        bn.addArc(x, y)

  names = cm.names()
  latentVarsDescriptor = list()
  lats = cm.latentVariablesIds()
  for latentVar in lats:
    inters = cm.children(latentVar) & nodes
    if len(inters) > 0:
      latentVarsDescriptor.append((names[latentVar],
                                   list(inters)))

  return CausalModel(bn, latentVarsDescriptor, True)